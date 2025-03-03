#include "Arduino.h"
#include <Adafruit_BME680.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HX711.h>
#include "Wire.h"
#include "drivers/RF95_Radio.h"
#include <SPI.h>
#include <RH_RF95.h>
#include <SD.h>

// standard C library
#include <string.h>

// FreeRTOS Library inclusion
#include "arduino_freertos.h"
#include "avr/pgmspace.h"
#include "semphr.h"

// Custom pinout for Teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

#define RF95_FREQ 915.0

// Initialize rf95 radio module
RF95_Radio rf95;

void setup(){
    Serial.begin(0);
    delay(2000);

    // Set pinout task
    pinMode(RFM95_RST, arduino::OUTPUT);
    digitalWrite(RFM95_RST, arduino::HIGH);

    Serial.println("Feather LoRa Rx Test!") ;

    // Manual reset
    digitalWrite(RFM95_RST, arduino::LOW);
    delay(10); 
    digitalWrite(RFM95_RST, arduino::HIGH);
    delay(10);
}

void loop(){
    // Inits a radio packet type
    radio_packet_t radio_packet;

    while (true){
        if (rf95._getMessage(1000, &radio_packet) == 0){
            Serial.println("Received something!");
            break;
        }
    }

    Serial.printf("Message received: %s", radio_packet.message);
}