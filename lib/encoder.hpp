#pragma once

#include <optional>

#include "codec.hpp"

class encoder : public codec {
private:
    uint8_t bufferEndBit;
    uint32_t dataVectorOffset_Index = 0;
    uint8_t bitsNotToEscape = 0;
    uint8_t previousNibble;

    void negateNibbleInBuffer(const uint8_t &startBit);

    void insertIntoBuffer(const command &command, const uint8_t &atBit);
    void insertIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

public:

    encoder(const uint8_t escapeSequence, std::vector<uint8_t> &dataVector);
    bool hasData();

    std::optional<uint8_t> nextByte();
    std::optional<uint8_t> nextNibble();
};