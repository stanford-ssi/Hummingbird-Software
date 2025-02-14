/*
 * Most recent update: Jan 17, 2025 by Sam C.
 * Fixed function definitions to be consistent with header file and resolved compilation issues.
 * This should allow the main.cpp file to compile and run on any platform.
 * The issues with RF module initialization still persists.
 */

#include "RF95_Radio.h"

// Custom pinout for teensy 4.1
#define RFM95_CS     10 // CS on module
#define RFM95_INT    0 // G0 on module
#define RFM95_RST    1 // RST on module
#define RFM95_IRQN   RFM69_INT

/*
 * the rest of the pins (Teensy --> Radio Module):
 * 3V3 --> Vin
 * G --> GND
 * EN can be left unconnected (defaults to be shorted with Vin)
 * SCK --> 13
 * MISO --> 12
 * MOSI --> 11
 */

#define BAUD_RATE 9600
#define ACK_BYTE !
#define SYNC_BYTE $
#define MAX_SYNC_TRIES 3

// Constructor
RF95_Radio::RF95_Radio(){
    Serial.begin(BAUD_RATE);

    RHHardwareSPI hardware_spi;
    RH_RF95 rf95 = RH_RF95(RFM95_CS, RFM95_INT, hardware_spi);  
    RF95_FREQ = 915.0;
    
    // this is not part of the original code and does nothing at the moment.
    // pin 1 is RFM95_RST (reset pin), which, when pulled low, should activate the radio module
    // but here it's not working for some reason
    digitalWrite(1, LOW);   // test

    // Initialize RF95
    // THIS IS WHERE THE CODE IS FAILING
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

// TODO: Create a packet header such that we only send a 
// header and then a payload during the radio protocol.
//
// [0:4] = Packet length
// [5:8] = Packet Type
void _calculatePacketHeader(){
   ; 
}

bool validate_check_byte(uint8_t *sync_bytes, uint8_t len, uint8_t byte_ref){
    for (int i  = 0; i < len; i++){
        if (sync_bytes[i] == byte_ref){
            return true;
        }
    }

    return false;
}

void _sendSync(){
    uint8_t data = SYNC_BYTE;
    
    // Sending the packet
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
}

bool _receiveSyn(){
    // Buffer for the sync bytes
    uint8_t sync_bytes[RF_RH95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(sync_bytes);

    for (int i = 0; i < MAX_SYNC_TRIES; i++){
        // Receive the pixels
        rf95.recv(sync_bytes, &len);

        if (validate_check_byte(sync_bytes, len, SYNC_BYTE)){
            return true;
        }
    }

    return false;
}


bool _receiveAck(){
    // Buffer for the sync bytes
    uint8_t ack_bytes[RF_RH95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(ack_bytes);

    for (int i = 0; i < MAX_SYNC_TRIES; i++){
        // Receive the pixels
        rf95.recv(ack_bytes, &len);

        if (validate_check_byte(ack_bytes, len, ACK_BYTE)){
            return true;
        }
    }

    return false;
} 

void _storeMessage(uint8_t *message, radio_packet_t *packet, int len){
    for (int i = 0; i < len; i++){
        packet->message[i] = message[i];
    }
}

void RF95_Radio::_getMessage(int bufferSize, radio_packet_t *packet){
    // Syncing the radios
    // Given that the _getMessage function will always
    // be called, this sync function is continually going
    // to be called until a sync is sent.
    if (!_receiveSyn()){
        return;
    }

    // Send an ACK that we received the sync!
    _sendACK();

    // Receive packet type
    uint8_t packet_type[RF_RH95_MAX_MESSAGE_LEN];
    uint8_t packet_type_len = sizeof(packet_type);
    if (rf95.recv(packet_type, packet_type_len)){
        Serial.println("Received packet type, storing packet type...");
        packet->packetType = packet_type[0];
    }

    // Send ACK that we received the packet type
    _sendACK();
    
    // Receive packet len
    uint8_t packet_length[RF_RH95_MAX_MESSAGE_LEN];
    uint8_t packet_length_len = sizeof(packet_length);
    if (rf95.recv(packet_length, packet_length_len)){
        Serial.println("Received packet length, storing packet type...");
        packet->packetLength = packet_length[0];
    }

    // Send ACK that we received the packet length
    _sendACK();
    
    // Receive packet message
    uint8_t packet_message[RF_RH95_MAX_MESSAGE_LEN];
    uint8_t packet_message_len = sizeof(packet_message);
    if (rf95.recv(packet_message, packet_message_len)){
        Serial.println("Received packet message, storing packet message..");
        _storeMessage(packet_message, packet);
    }

    // Send ACK that we received the message
    _sendACK();
}

void RF95_Radio::_sendMessage(uint8_t packetLength, radio_packet_t radio_packet){
    // Syncing the radios together
    _sendSync();
    if (!_receiveAck()){
        Serial.println("Sync was not acknowledged!");
    }

    // Sending the packet type
    rf95.send(radio_packet->packetType, sizeof(uint8_t));
    rf95.waitPacketSent();
    if (!_receiveAck()){
        Serial.println("Packet type was not acknowledged!");
    }
   
    // Sending the packet length
    rf95.send(radio_packet->packetLength, sizeof(uint8_t));
    rf95.waitPacketSent();
    if (!_receiveAck()){
        Serial.println("Packet length was not acknowledged!");
    }

    // Sending the packet message
    rf95.send((uint8_t *)radio_packet->message, radio_packet->packetLength);
    rf95.waitPacketSent();
    if (!_receiveAck()){
        Serial.println("Packet message was not acknowledged!");
    }
}

void RF95_Radio::_changeFrequency(double freq){
        RF95_FREQ = freq;
    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("Setting new frequency failed!");
    }
}

void RF95_Radio::_sendACK(){
    // ACK message
    uint8_t data = ACK_BYTE;
    
    // Sending the packet
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
}
