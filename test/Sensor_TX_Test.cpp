// sensor_TX_Test.cpp

/*
Testing script designed for teensy 4.1 connected to 900MHz LoRa radio module on SPI and BME680 on I2C
Teensy SPI pinout:
SCK: 13
MISO: 12
MOSI: 11
CS: 10
Interrupt: 0
Rst: 1
Vin: Vin
Gnd: Gnd

Teensy I2C pinout:
V3.3: V3.3
Gnd: Gnd
SDA: 18
SCL: 19
*/

#include "Arduino.h"
#include <Adafruit_BME680.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include "Wire.h"
#include <SPI.h>
#include <RH_RF95.h>

// SPI pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

// Must match RX frequency
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
Adafruit_BME680 bme;

void setup() {
  // configure pin inputs/outputs.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  while (!Serial) delay(1);
  delay(100);

  Serial.println("Teensy 4.1 Sensor Data Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    // Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);

  // I2C init
  bme.begin();
}

int16_t packetnum = 0;

void loop() {
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!

  // I2C reading from BME680
  float temp = bme.readTemperature();

  // Serial.print("Temp: ");
  // Serial.println(temp);
  // snprintf(buffer, sizeof(buffer), "The value of heat is: %.2f\n", temp);

  
  Serial.println("Transmitting..."); // Send a message to rf95_server
  char radiopacket[20] = "Temp:              ";
  snprintf(radiopacket + 6, sizeof(radiopacket) - 6, "%.2f\n", temp);

  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[19] = 0;

  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  Serial.println("Waiting for packet to complete...");
  delay(10);
  rf95.waitPacketSent();
  Serial.println("Hopefully message was recieved");
  delay(1000);

}