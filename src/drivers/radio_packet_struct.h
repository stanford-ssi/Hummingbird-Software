#pragma once

#include <string>

typdef struct _radio_packet_t{
    uint8_t packetType;
    uint8_t packetLength;
    std::message message;
} radio_packet_t;

