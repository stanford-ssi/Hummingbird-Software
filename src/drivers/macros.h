#pragma once

#include <string>

typedef struct _packet_header_t{
    uint8_t packetType;
    int packetLength;
} packet_header_t;

typdef struct _radio_packet_t{
    uint8_t packetType;
    uint8_t packetLength;
    uint8_t *message;
} radio_packet_t;

