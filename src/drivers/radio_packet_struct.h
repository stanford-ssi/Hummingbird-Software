#pragma once

#include <string>

typdef struct _radio_packet_t{
    uint8_t packetType;
    uint8_t packetLength;
    uint8_t *message;
} radio_packet_t;

