#pragma once

#include "Arduino.h"
#include "Wire.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <RH_RF95.h>

class RF95_Radio(){
publc: 
    /* Initializes the pins and ports of the rf95 */
    RF95_Radio();

    /* Tries to find the message, and prints out the message's output */
    void _getMessage(int bufferSize);

    /* Sends a message to the receiving radio */
    void _sendMessage(uint8_t packetLength);

    /* Changes the radio's frequency*/
    void _changeFrequency(double freq);

private:
    /* Returns an ACK to the transmitter */
    void _sendACK();

    /* Singleton instance of the radio driver */
    RH_RF95 rf95;

    /* Radio frequency */
    double RF95_FREQ = 0;

    /* Keeps count of packets sent */
    int16_t packetCount = 0;
}
