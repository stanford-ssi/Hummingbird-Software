// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

// 900Mhz_TX_Test.cpp

#include "Arduino.h"
#include <Adafruit_BME680.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include "Wire.h"
#include <SPI.h>
#include <RH_RF95.h>

#include <strings.h>

// Custom pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

// Change to 434.0 or other frequency, must match RX's freq!
// #define RF95_FREQ 434.0
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial) delay(1);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop() {
  Serial.println("Enter message to send:");
  while (Serial.available() == 0) {
    // Wait for user input
  }
  
  char radiopacket[RH_RF95_MAX_MESSAGE_LEN];
  int index = 0;

  if (Serial.available() > 0 && index < sizeof(radiopacket - 1)) {
     while (Serial.available() > 0 && index < sizeof(radiopacket) - 1) {
      char c = Serial.read();
      if (c == '\n') break;  // Stop reading at newline
      radiopacket[index++] = c;
    }
    radiopacket[index] = '\0';  // Null-terminate the string
    
    char buf_to_comp_to[1] = {'t'};
    if (strncmp(radiopacket, buf_to_comp_to, 1) == 0) {
      Serial.println("Are you sure? Toggling at next toggle command");
    }


    while (Serial.available() > 0 && index < sizeof(radiopacket) - 1) {
      char c = Serial.read();
      if (c == '\n') break;  // Stop reading at newline
      radiopacket[index++] = c;
    }
    radiopacket[index] = '\0';  // Null-terminate the string

    if (strncmp(radiopacket, buf_to_comp_to, 1) == 0) {
      Serial.print("Sending: "); Serial.println(radiopacket);
      rf95.send((uint8_t *)radiopacket, index + 1);
      rf95.waitPacketSent();
      Serial.println("Message sent!");
    }
  }

  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000)) {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len)) {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    } else {
      Serial.println("Receive failed");
    }
  } else {
    Serial.println("No reply, is there a listener around?");
  }
}