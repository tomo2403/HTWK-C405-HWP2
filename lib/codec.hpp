#pragma once

#include "global.hpp"

class codec
{
protected:
    uint32_t buffer = 0x00000000;
    
    const uint8_t escapeSequence = 0x80;
    std::vector<uint8_t> &dataVector;

    uint8_t bufferEndBit;

    bool hasNegatedNibbles(const uint8_t &byte);
    bool hasEqualNibbles(const uint8_t &byte);
    uint8_t negateLowNibble(const uint8_t &byte);
    void leftShiftByteIntoBuffer(const uint8_t &byte);
    uint8_t getByteSlice(const uint8_t &startBit);
    uint8_t getNibbleSlice(const uint8_t &startBit);
    bool areNegated(const uint8_t &nibbleOne, const uint8_t &nibbleTwo);
    void leftShiftNibbleIntoBuffer(const uint8_t &byte);

public:
    codec(const uint8_t escapeSequence, std::vector<uint8_t> &dataVector);
    
    // Sodnerfall nachfolgendes Nibble ist gleich command
    void zeroBuffer();
    enum command {
        preserveNextByteDefault = 0x01,
        preserveNextByteFallback = 0x02,
        iAmReady = 0x03
    };
};