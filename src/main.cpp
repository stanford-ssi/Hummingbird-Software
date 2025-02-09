// 900Mhz_RX_Test.cpp

#include "Arduino.h"
#include <Adafruit_BME680.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include "Wire.h"
#include <SPI.h>
#include <RH_RF95.h>
#include <SD.h>

// standard C library
#include <string.h>

// LED pin definition
#define LED_PIN 6

// Custom pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
const int chipSelect = BUILTIN_SDCARD;
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
File myFile;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);


  while (!Serial) delay(1);
    /*
    Serial.println("initialiing SD card");
    if (!SD.begin(chipSelect)){


        Serial.println("initialization failed");
        return;
    }
    Serial.println("initialization done");
    delay(100);
    */
    // myFile = SD.open("sensorData.txt", FILE_WRITE);
    Serial.println("Feather LoRa RX Test!");

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

  // set pinmode of LED pin to output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (rf95.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      digitalWrite(LED_BUILTIN, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");

      Serial.println((char*)buf);


      char buffer1[140];
      snprintf(buffer1, sizeof(buffer1), "The value of the received message is: %s\n", (char*)buf);

      char buffer_char[140];
      snprintf(buffer_char, sizeof(buf), "%s\n", (char*)buf);
      Serial.print("buffer: ");
      Serial.println(buffer_char);
      Serial.println(strncmp(buffer_char, "toggle", 6));

      // checking for command receiving
      char buf_to_comp_to[6] = {'t', 'o', 'g', 'g', 'l', 'e'};
      if (strncmp(buffer_char, buf_to_comp_to, 6) == 0) {
        int cur_state = digitalRead(LED_PIN);
        digitalWrite(LED_PIN, !cur_state);
        Serial.println("toggling...");
      }
      /*
    // writing to the SD card
     myFile.write(buffer1);
    myFile.flush();
      */

       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      Serial.println("Receive failed");
    }
  }
}
