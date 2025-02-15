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

// arduino freertos library inclusion
#include "arduino_freertos.h"
#include "avr/pgmspace.h"
#include "semphr.h"

// Custom pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

// Change to 434.0 or other frequency, must match RX's freq!
// #define RF95_FREQ 434.0
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
static RH_RF95 rf95(RFM95_CS, RFM95_INT);

static int16_t packetnum = 0;  // packet counter, we increment per xmission

// define mutex for locking Serial
static SemaphoreHandle_t mutex;

// this task handles the sending of radio packets
static void radioSendTask(void *param) {
  Serial.begin(0);
  char radiopacket[RH_RF95_MAX_MESSAGE_LEN];
  int index = 0;
  
  while (1) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    Serial.println("Enter message to send:");
    xSemaphoreGive(mutex);
    while (Serial.available() == 0) {
      // Wait for user input
    }

    while (Serial.available() > 0 && index < sizeof(radiopacket) - 1) {
      char c = Serial.read();
      if (c == '\n') break;  // Stop reading at newline
      radiopacket[index++] = c;
    }
    radiopacket[index] = '\0';  // Null-terminate the string

    // critical section to protect the Serial peripheral
    xSemaphoreTake(mutex, portMAX_DELAY);
    Serial.print("Sending: "); 
    Serial.println(radiopacket);
    rf95.send((uint8_t *)radiopacket, index + 1);
    if (!rf95.waitPacketSent(10000)) {
        xSemaphoreGive(mutex); // 1000 ms timeout
        Serial.println("ERROR");
        vTaskDelete(NULL);
    }
    Serial.println("Message sent!");
    xSemaphoreGive(mutex);

    index = 0;
  }
}

// this task handles the reading of pressure transducers
static void radioReceiveTask(void *param) {  
  Serial.begin(0);
  
  while (1) {
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    xSemaphoreTake(mutex, portMAX_DELAY);
    Serial.println("Waiting for reply...");
    xSemaphoreGive(mutex);
    
    if (rf95.waitAvailableTimeout(1000)) {
      // Should be a reply message for us now
      xSemaphoreTake(mutex, portMAX_DELAY);
      if (rf95.recv(buf, &len)) {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), arduino::DEC);
        xSemaphoreGive(mutex);
      } else {
        Serial.println("Receive failed");
        xSemaphoreGive(mutex);
      }
    } else {
      xSemaphoreTake(mutex, portMAX_DELAY);
      Serial.println("No reply, is there a listener around?");
      xSemaphoreGive(mutex);
    }
  }

}

FLASHMEM __attribute__((noinline)) void setup() {
  Serial.begin(0);
  delay(2'000);
  
  pinMode(RFM95_RST, arduino::OUTPUT);
  digitalWrite(RFM95_RST, arduino::HIGH);

  /*while (!Serial) delay(1);
  delay(100);*/

  Serial.println("Feather LoRa TX Test!");
  Serial.flush();

  // manual reset
  digitalWrite(RFM95_RST, arduino::LOW);
  delay(10);
  digitalWrite(RFM95_RST, arduino::HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.flush();
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    Serial.flush();
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  Serial.flush();

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    Serial.flush();
    while (1);
  }
  Serial.print("Set Freq to: "); 
  Serial.flush();
  Serial.println(RF95_FREQ);
  Serial.flush();
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  // the remaining is FreeRTOS setup
  /*
  BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
                         const char * const pcName,
                         const configSTACK_DEPTH_TYPE uxStackDepth,
                         void *pvParameters,
                         UBaseType_t uxPriority,
                         TaskHandle_t *pxCreatedTask
                       );
  */

  // create mutex before starting tasks
  mutex = xSemaphoreCreateMutex();

  xTaskCreate(radioSendTask, "radioSendTask", 1024, NULL, 1, NULL);  // priority 1
  xTaskCreate(radioReceiveTask, "radioReceiveTask", 1024, NULL, 1, NULL);    // priority 1

  Serial.println("setup(): starting scheduler...");
  Serial.flush();

  vTaskStartScheduler();
}

void loop() {}