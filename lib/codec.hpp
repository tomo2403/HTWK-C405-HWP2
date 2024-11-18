#pragma once

#include <cstdint>
#include <vector>

class codec
{
protected:
    uint32_t buffer = 0x00000000;
    
    const uint8_t escapeSequence = 0x80;
    std::vector<uint8_t> &dataVector;
    uint8_t previousNibble = 0x10;

    uint8_t bufferEndBit{};

    static bool hasNegatedNibbles(const uint8_t &byte);
    static bool hasEqualNibbles(const uint8_t &byte);
    static uint8_t negateLowNibble(const uint8_t &byte);
    void leftShiftByteIntoBuffer(const uint8_t &byte);
    uint8_t getByteSlice(const uint8_t &startBit) const;
    uint8_t getNibbleSlice(const uint8_t &startBit) const;
    static bool areNegated(const uint8_t &nibbleOne, const uint8_t &nibbleTwo);
    void leftShiftNibbleIntoBuffer(const uint8_t &byte);
    void negateNibbleInBuffer(const uint8_t &startBit);

public:
    codec(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);
    
    // Sonderfall nachfolgendes Nibble ist gleich command
    void zeroBuffer();
    enum command {
        preserveNextByteDefault = 0x01,
        preserveNextByteFallback = 0x02,
        iAmReady = 0x03,
		everythingSend = 0x04,
    };
};