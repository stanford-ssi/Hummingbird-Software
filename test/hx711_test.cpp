#include "Arduino.h"
#include <Adafruit_BME680.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HX711.h>
#include "Wire.h"
#include <SPI.h>
#include <RH_RF95.h>
#include <SD.h>

// Define the pins for the HX711 communication (load cell)
const uint8_t DATA_PIN = 2;  // Can use any pins!
const uint8_t CLOCK_PIN = 3; // Can use any pins!

/* * * * * HX711 (Load Cell) Initialization * * * * */
Adafruit_HX711 hx711(DATA_PIN, CLOCK_PIN);

void setup(){
    Serial.begin(9600);

    /* * * * * HX711 Set-Up * * * * */
    hx711.begin();

    // Honestly don't know what the bottom does.
    // Just uncomment if needed
     // read and toss 3 values each
     Serial.println("Tareing....");
     for (uint8_t t=0; t<3; t++) {
        hx711.tareA(hx711.readChannelRaw(CHAN_A_GAIN_64));
        hx711.tareA(hx711.readChannelRaw(CHAN_A_GAIN_64));
        hx711.tareB(hx711.readChannelRaw(CHAN_B_GAIN_32));
        hx711.tareB(hx711.readChannelRaw(CHAN_B_GAIN_32));
    }
}

void loop() {
    /* * * * * HX711 * * * * */
    // Read from Channel A with Gain 128, can also try CHAN_A_GAIN_64 or CHAN_B_GAIN_32
    // since the read is blocking this will not be more than 10 or 80 SPS (L or H switch)
    // int32_t weightA128 = hx711.readChannelBlocking(CHAN_A_GAIN_64);
    // Serial.print("Channel A (Gain 64): ");
    // Serial.println(weightA128);

    // Read from Channel A with Gain 128, can also try CHAN_A_GAIN_64 or CHAN_B_GAIN_32
    int32_t weightB32 = hx711.readChannelBlocking(CHAN_B_GAIN_32);
    Serial.print("Channel B (Gain 32): ");
    Serial.println(weightB32);

}