/* 
    multiSensor.cpp
    Polling multiple I2C devices and building a packet to send over
    SPI using LoRa radio
*/

#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_MCP9600.h>
#include <Adafruit_Sensor.h>
#include <RH_RF95.h>

// Custom pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

/* Set and print ambient resolution */
Ambient_Resolution ambientRes = RES_ZERO_POINT_0625;

Adafruit_ICM20948 icm;
Adafruit_MCP9600 mcp;
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

    // Initialize LoRa module
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);

    if (!rf95.init()) {
        Serial.println("LoRa initialization failed");
        while (1);
    }

    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
    }
    rf95.setTxPower(23, false);
    Serial.println("LoRa initialized");

    // Initialize ICM20948
    if (!icm.begin_I2C()) {
        Serial.println("Failed to find ICM20948");
        while (1);
    }

    Serial.println("ICM20948 Found!");

    // Initialize MCP9600
    if (!mcp.begin(0x67)) {
        Serial.println("Failed to find MCP9600");
        while (1);
    }
    Serial.println("MCP9600 Found");

    /* Set and print ambient resolution */
    mcp.setAmbientResolution(ambientRes);
    mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
    mcp.setThermocoupleType(MCP9600_TYPE_K);
    mcp.setFilterCoefficient(3);
    mcp.enable(true);
}

void loop() {
    // Read ICM20948 sensor data
    sensors_event_t accel, gyro, mag, temp;
    icm.getEvent(&accel, &gyro, &temp, &mag);
    
    // Read MCP9600 temperature
    float mcp_temp = mcp.readThermocouple();
    
    // Prepare data buffer
    uint8_t buffer[255];
    int index = 0;
    
    // Pack accelerometer data
    memcpy(&buffer[index], &accel.acceleration.x, sizeof(float)); index += sizeof(float);
    memcpy(&buffer[index], &accel.acceleration.y, sizeof(float)); index += sizeof(float);
    memcpy(&buffer[index], &accel.acceleration.z, sizeof(float)); index += sizeof(float);
    
    // Pack gyroscope data
    memcpy(&buffer[index], &gyro.gyro.x, sizeof(float)); index += sizeof(float);
    memcpy(&buffer[index], &gyro.gyro.y, sizeof(float)); index += sizeof(float);
    memcpy(&buffer[index], &gyro.gyro.z, sizeof(float)); index += sizeof(float);
    
    // Pack magnetometer data
    memcpy(&buffer[index], &mag.magnetic.x, sizeof(float)); index += sizeof(float);
    memcpy(&buffer[index], &mag.magnetic.y, sizeof(float)); index += sizeof(float);
    memcpy(&buffer[index], &mag.magnetic.z, sizeof(float)); index += sizeof(float);
    
    // Pack temperature data
    memcpy(&buffer[index], &mcp_temp, sizeof(float)); index += sizeof(float);
    
    // Transmit data over LoRa
    rf95.send(buffer, index);
    rf95.waitPacketSent();
    
    // Print buffer to serial
    Serial.print("Sent Buffer: ");
    for (int i = 0; i < index; i++) {
        Serial.printf("%02X ", buffer[i]);
    }
    Serial.println();
    
    delay(1000);
}