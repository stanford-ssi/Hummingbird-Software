#pragma once

#include "Arduino.h"
#include "Wire.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <RH_RF95.h>

/* Credits: 
 * Code was heavily inspired by SSI Samwise Payload 2024 UART Protocol.
 */

/* Purpose:
 * This entire object just acts as a wrapper for the RH_RF95 module. It simplifies
 * having to transmit and receive messages. At the time you are reading this it is
 * currently not a true wrapper since you still have to pass through a RF_RH95*.
 * Will eventually fix it soon after finals.
 */
class RF95_Radio {
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
    /* How it works: 
     * --> Calculates a header. The header contains the length of the actual message
     *  being sent, and a sequence number.
     * --> The header is sent to a receiver. The header is sent first before the packet
     *  to ensure that the receiver is ready to receive the packet. In addition, it
     *  gives the receiver additional information about the packet that would help it
     *  verify the integrity of the packet.
     * --> Waits for an acknowledgement that it was received.
     * --> Sends the actual packet to the receiver.
     *
     * Current limitations:
     * --> Does not verify the actual acknowledgement being sent, it could just be 
     *  garbage data.
     * --> Does wait and listen for an acknowledgement that the packet was received.
     */
    void _sendMessage(const uint8_t *packet, uint16_t len, uint16_t seq_num);

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
