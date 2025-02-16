#include "Command_Handler.h"

Command_Handler::Command_Handler(){
    RF95_Radio radio; 
}

void Command_Handler::_sendPacket(radio_packet_t packet){
    (void) packet;
}

void Command_Handler::_recvPacket(){
    ;
}

void Command_Handler::_processPacket(radio_packet_t packet){
    (void) packet;
}

void Command_Handler::_executeCommand(radio_packet_t packet){
    (void) packet;
}

void Command_Handler::_storeData(radio_packet_t packet){
    (void) packet;
}

radio_packet_t _encodeProcess(uint8_t packetType, uint8_t packetLength, std::string message){
    (void) packetType;
    (void) packetLength;
    (void) message;
}

