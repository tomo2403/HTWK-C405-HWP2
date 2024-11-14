#include "codec.hpp"
#include <iostream>
#include <bitset>

codec::codec(const uint8_t escapeSequence, std::vector<uint8_t> &dataVector)
    : escapeSequence(escapeSequence), dataVector(dataVector)
{}

bool codec::hasNegatedNibbles(const uint8_t& byte) {
    uint8_t highNibble = byte >> 4;
    uint8_t negatedLowNibble = ~byte & 0x0F;

    return highNibble == negatedLowNibble;
}

bool codec::hasEqualNibbles(const uint8_t& byte) {
    uint8_t highNibble = byte >> 4;
    uint8_t lowNibble = byte & 0x0F;

    return highNibble == lowNibble;
}

uint8_t codec::negateLowNibble(const uint8_t& byte) {
    uint8_t highNibble = byte | 0x0F;
    uint8_t lowNibble = byte & 0x0F;

    return highNibble & ~lowNibble;
}

void codec::leftShiftByteIntoBuffer(const uint8_t &byte)
{
    buffer <<= 8;
    buffer |= byte;
}

void codec::leftShiftNibbleIntoBuffer(const uint8_t &byte)
{
    buffer <<= 4;
    buffer |= (byte & 0x0F);
}

void codec::zeroBuffer()
{
    buffer = 0x00000000;
}

uint8_t codec::getByteSlice(const uint8_t &startBit)
{   
    if (startBit < 0 && startBit > 24) {
        throw std::invalid_argument("Start must define an 8-bit slice within the buffer.");
    }

    return static_cast<uint8_t>((buffer >> startBit) & 0x000000FF);
}

uint8_t codec::getNibbleSlice(const uint8_t &startBit)
{
    if (startBit < 0 && startBit > 28) {
        throw std::invalid_argument("Start must define an 4-bit slice within the buffer.");
    }

    return static_cast<uint8_t>((buffer >> startBit) & 0x0000000F);
}

bool codec::areNegated(const uint8_t &nibbleOne, const uint8_t &nibbleTwo)
{
    return ((~nibbleOne) & 0x0F) == nibbleTwo;
}