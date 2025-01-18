/*
 * Most recent update: Jan 17, 2025 by Sam C.
 * Fixed function definitions to be consistent with implementation (cpp) file and resolved compilation issues.
 * This should allow the main.cpp file to compile and run on any platform.
 * The issues with RF module initialization still persists.
 */

#pragma once

#include "Arduino.h"
#include "Wire.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <RH_RF95.h>

#include <string>

using namespace std;

class RF95_Radio
{
public: 
    /* Initializes the pins and ports of the rf95 */
    RF95_Radio(RH_RF95*);

    /* Tries to find the message, and prints out the message's output */
    /* How it works: 
     * --> Continually listens for a header to be sent.
     * --> Processes the header.
     * --> Sends an acknowledgement that it received the header.
     * --> Listens for a packet to be sent.
     * --> Stores the packet.
     *
     * Current limitations:
     * --> Does not time out at a certain amount of time has passed listening for
     *  the packet.
     * --> Does not check the integrity of the packet and header, so if bytes
     *  are missing it will be missed.
     * --> Does not send an acknowledgement that it received the packet.
     */
    void _getMessage(uint8_t *packet);

    /* Sends a message to the receiving radio */
    void _sendMessage(uint8_t packetLength, std::string message);

    /* Changes the radio's frequency*/
    void _changeFrequency(double freq);

private:
    /* Returns an ACK to the transmitter */
    void _sendAck();

    /* Singleton instance of the radio driver */
    RH_RF95* radio;

    /* Radio frequency */
    double RF95_FREQ = 0;

    /* Keeps count of packets sent */
    int16_t packetCount = 0;
};
