#pragma once

#include <cstdint>
#include <vector>

#include "Packets.hpp"

class Codec
{
protected:
    uint32_t buffer = 0x00000000;
    
    const uint8_t escapeSequence = 0x00;
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
    Codec();
    
    // Sonderfall nachfolgendes Nibble ist gleich command
    void zeroBuffer();
};

enum CodecCommand {
    // =========================================================================
    // WARNING: Do not use 0x00, as it represents the escape sequence itself!
    // NOTE: Every command is preceded by the escape sequence before being sent.
    // =========================================================================

    // If a nibble occurs back-to-back in the data stream, it is only sent once,
    // followed by this command.
    insertPrevNibbleAgainDefault = 0x01,

    // If the upcoming nibble after the command is the same as the default command-nibble,
    // this fallback command is used to avoid sending the same nibble twice in succession.
    insertPrevNibbleAgainFallback = 0x02,
	
    // The communication partner is ready to send/receive
    iAmReady = 0x03,
	
    // This command marks the end of a data-block.
	endBlock = 0x04,

    // This command marks the begin of a data-block.
    beginBlockDefault = 0x05,

    // If the upcoming nibble after the command is the same as the default command-nibble,
    // this fallback command is used to avoid sending the same nibble twice in succession.
    beginBlockFallback = 0x08,
    
    // If the escape sequence occurs in the data stream, this command is placed after
    // it to inform the decoder to place the escape sequence back in the original data stream.
    insertEscSeqAsDataDefault = 0x06,

    // If the upcoming nibble after the command is the same as the default command-nibble,
    // this fallback command is used to avoid sending the same nibble twice in succession.
    insertEscSeqAsDataFallback = 0x07,

	closeConnection = 0x09,

	everythingSend = 0x0A
};
