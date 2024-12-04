#include "RF95_Radio.h"
#include <string>

// Custom pinout for teensy 4.1
#define RFM95_CS     10  // "B"
#define RFM95_INT    0 // "C"
#define RFM95_RST    1  // "A"
#define RFM95_IRQN   RFM69_INT

#define BAUD_RATE 9600

#define MAX_PACKET_LEN 4069

#define ACK_TRIES 3
    
typedef struct{
    uint16_t length;
    uint16_t seq_num;
    uint32_t checksum;
} packet_header_t;

unsigned int crc32(const uint8_t *message, uint16_t len){
    int i, j;
    unsigned int byte, crc, mask;

    i = 0;
    crc = 0xFFFFFFFF;
    while (i < len){
        byte = message[i];
        crc = crc ^ byte;
        
        for (j = 7; j >= 0; j--)
        { // Do eight times.
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        i = i + 1;
    }
    return ~crc;
}

packet_header_t compute_packet_header (const uint8_t *packet, uint16_t len, uint16_t seq_num){
    packet_header_t header = {
        len,
        seq_num,
        crc32(packet, len)
    };

    return header;
}

// Constructor
RF95_Radio::RF95_Radio(){
    Serial.begin(BAUD_RATE);

    RF95_FREQ = 915.0;

    // Initialize RF95
    while (!rf95.init()) {
        Serial.println("LoRa radio init failed");
        Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    }
    Serial.println("LoRa radio init OK!");

    // Initialize frequency
    while (!rf95.setFrequency(RF95_FREQ)){
        Serial.println("Frequency was not set, error!"); 
    }
    Serial.println("Succesfully set frequency to: ");
    Serial.print(RF95_FREQ);

    // Setting Transmit power
    rf95.setTxPower(23, false);

    /* Note:
        * Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
        * The default transmitter power is 13dBm, using PA_BOOST.
        * If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
        * you can set transmitter powers from 5 to 23 dBm.
        */
}

void RF95_Radio::_getMessage(int bufferSize){
    const int secondBufferSize = 140;
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
        digitalWrite(LED_BUILTIN, HIGH);
        RH_RF95::printBuffer("Received: ", buf, len);
        Serial.print("Got: ");

        Serial.println((char*)buf);

        char secondBuffer[secondBufferSize];
        snprintf(secondBuffer, sizeof(secondBuffer), "The value of the received message is: %s\n", (char*)buf);

        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);

        _sendAck();
    } else {
        Serial.println("Receive failed!");
    } 
}

void RF95_Radio::_sendMessage(const uint8_t *packet, uint16_t len, uint16_t seq_num){
    // Check packet length
    if (len > MAX_PACKET_LEN){
        printf("Packet is too long!\n");
        return;
    }

    // Calculate the header
    Serial.println("Fixing the packet...");
    packet_header_t header = compute_packet_header(packet, len, seq_num);

    // Sending the header
    Serial.println("Sending the header...");
    rf95.send((uint8_t *) &header, sizeof(packet_header_t));

    Serial.println("Waitinf for header to completely send...");
    rf95.waitPacketSent(); // Blocking?

    // ACK for header
    uint8_t numTries = 0;
    while (numTries > ACK_TRIES){
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)){
            Serial.println("Header ACK received!");
        }
    } 

    if (numTries >= ACK_TRIES){
        Serial.println("Header was not acknowledged!");
        return;
    }

    // Sending the actual packet
    Serial.println("Sending the packet...");
    rf95.send(packet, len);

    Serial.println("Waiting for packet to completely send...");
    rf95.waitPacketSent(); // Blocking?
}

void RF95_Radio::_changeFrequency(double freq){
        RF95_FREQ = freq;
    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("Setting new frequency failed!");
    }
}

void RF95_Radio::_sendAck(){
    // ACK message
    uint8_t data[] = "And hello back to you";
    
    // Sending the packet
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    Serial.println("Sent a reply");
    digitalWrite(LED_BUILTIN, LOW);
}
