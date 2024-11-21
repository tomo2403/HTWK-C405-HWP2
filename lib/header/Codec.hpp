#pragma once

#include <cstdint>
#include <vector>

#include "Packets.hpp"

class Codec
{
protected:
    uint32_t buffer = 0x00000000;
    
    const uint8_t escapeSequence = 0x00;
    std::vector<uint8_t> &dataVector;
    uint8_t previousNibble = 0x00;

    int8_t bufferEndBit = -1;

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
    Codec(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);
    
    // Sonderfall nachfolgendes Nibble ist gleich command
    void zeroBuffer();
};

enum CodecCommand {
    // Do NOT use: 0x00, 0xFF!
	insertPrevNibbleAgainDefault = 0x01,
    insertPrevNibbleAgainFallback = 0x02,
	/** The communication partner is ready to send/receive */
	iAmReady = 0x03,
	/** The communication partner has sent everything */
	STOP = 0x04,
    onlyReadOneBitOfNextNibble = 0x05,
    insertEscSeqAsDataDefault = 0x06,
    insertEscSeqAsDataFallback = 0x07,
};
