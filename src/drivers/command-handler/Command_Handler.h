#pragma once

#include "drivers/RF95_Radio.h"
#include "drivers/radio_packet_struct.h"
#include <string>

using namespace std;


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
