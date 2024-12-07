#pragma once

#include <cstdint>
#include <vector>
#include <mutex>

class Codec
{
protected:
	mutable std::mutex mtx;
    uint32_t buffer;
    bool previousNibbleExists;
    uint8_t previousNibble;
    int8_t bufferEndBit;

    void initialize();

    static bool hasNegatedNibbles(const uint8_t &byte);
    static bool hasEqualNibbles(const uint8_t &byte);
    static uint8_t negateLowNibble(const uint8_t &byte);
    void leftShiftByteIntoBuffer(const uint8_t &byte);
    [[nodiscard]] uint8_t getByteSlice(const uint8_t &startBit) const;
    [[nodiscard]] uint8_t getNibbleSlice(const uint8_t &startBit) const;
    static bool areNegated(const uint8_t &nibbleOne, const uint8_t &nibbleTwo);
    void leftShiftNibbleIntoBuffer(const uint8_t &byte);
    void negateNibbleInBuffer(const uint8_t &startBit);
    void gracefullyInsertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &startBit);
    uint8_t currentNibble();
    uint8_t currentByte();

public:
    Codec();
};