#pragma once

#include "drivers/RF95_Radio.h"
#include <string>

using namespace std;

typedef struct _radio_packet_t{
    uint8_t packetType;
    uint8_t packetLength;
    std::string message;
}radio_packet_t;

class Command_Handler{
    public: 
        Command_Handler();
        
        void _sendPacket(radio_packet_t packet);

        void _recvPacket();

        void _processPacket(radio_packet_t packet);

        void _executeCommand(radio_packet_t packet);

        void _storeData(radio_packet_t packet);


    private:
        /* Returns  */
        radio_packet_t _encodeProcess(uint8_t packetType, uint8_t packetLength, std::string message);
}
