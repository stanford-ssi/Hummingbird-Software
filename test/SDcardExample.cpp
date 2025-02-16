#include "Arduino.h"
#include <Adafruit_BME680.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include "Wire.h"
#include <SD.h>

// analog inputs: 
// PT: pressure transducer
// LOAD: load cell
#define PT_INPUT A0
#define LOAD_IN_1 A10
#define LOAD_IN_2 A11

char buffer[50];
// digital inputs via I2C: BME680 (can read temp, pressure, humidity, gas)
// Note from Sam: I was too lazy (and don't have time) to configure the I2C setup from the ground up.
// It can be done, but will need someone with substantial serial communication knowledge and lots of time on their hands.
Adafruit_BME680 bme;    // initializes I2C object
File myFile;
const int chipSelect = BUILTIN_SDCARD;
void setup() {
    // Teensy 4.1 default baud rate is 9600, or 6 Mbits/s
    Serial.begin(9600);
    Serial.println("initialiing SD card");
    if (!SD.begin(chipSelect)){
        Serial.println("initialization failed");
        return;
    }
    Serial.println("initialization done");
    
    // configure pin inputs/outputs.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PT_INPUT, INPUT);
    pinMode(LOAD_IN_1, INPUT);
    pinMode(LOAD_IN_2, INPUT);
    myFile = SD.open("sensorData.txt", FILE_WRITE);

    // begin I2C object. See Adafruit.BME680.h for interface details
    bme.begin();
}

// the loop function runs over and over again forever
void loop() {

    // I2C reading from BME680
    float temp = bme.readTemperature();
    Serial.print("Temp: ");
    Serial.println(temp);
    snprintf(buffer, sizeof(buffer), "The value of heat is: %.2f\n", temp);
    myFile.write(buffer);

    delay(50);    // delay 50ms
    myFile.flush();
    
}
