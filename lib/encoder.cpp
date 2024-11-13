#include <iostream>
#include <bitset>

#include "encoder.hpp"

encoder::encoder(const uint8_t escapeSequence, std::vector<uint8_t> &dataVector) : codec::codec(escapeSequence, dataVector)
{
    if (dataVector.size() == 0)
    {
        throw std::invalid_argument("Data-Vector cannot be empty.");
    }
    else
    {
        leftShiftByteIntoBuffer(dataVector.at(dataVectorOffset_Index++));
        bufferEndBit = 7;
    }
}

void encoder::insertIntoBuffer(const command &command, const uint8_t &atBit)
{
    if (atBit > 20)
    {
        throw std::out_of_range("The buffer is 32bit in size and the sequence 12bit long.");
    }

    buffer &= ~(0xFFF << atBit);
    buffer |= (static_cast<uint32_t>(escapeSequence) << (atBit + 4));
    buffer |= (static_cast<uint32_t>(command) << atBit);
}

void encoder::insertIntoBuffer(const uint8_t &byte, const uint8_t &atBit)
{
    if (atBit < 0 || atBit > 24)
    {
        throw std::out_of_range("The buffer is 32bit in size and the sequence 8bit long.");
    }

    buffer &= ~(0xFF << atBit);
    buffer |= (static_cast<uint32_t>(byte) << (atBit));
}

bool encoder::hasData()
{
    return bufferEndBit != 0 || dataVectorOffset_Index < dataVector.size();
}

void encoder::negateNibbleInBuffer(const uint8_t &startBit)
{

    if (startBit > 28) {
        throw std::out_of_range("Start bit cannot be higher than 28.");
    }

    const uint8_t originalNibble = static_cast<uint8_t>((buffer >> startBit) & 0x0F);
    const uint8_t negatedNibble = ~originalNibble & 0x0F;

    const uint32_t mask = ~(0x0F << startBit);

    buffer &= mask;
    buffer |= (static_cast<uint32_t>(negatedNibble) << startBit);
}

std::optional<uint8_t> encoder::nextNibble()
{
    if (bufferEndBit <= 3)
    {
        if (dataVectorOffset_Index >= dataVector.size())
        {
            bufferEndBit = 0;
            return static_cast<uint8_t>(buffer & 0x0F);
        }
        else
        {
            leftShiftByteIntoBuffer(dataVector.at(dataVectorOffset_Index++));
            bufferEndBit += 8;
        }
    }

    if (hasNegatedNibbles(getByteSlice(bufferEndBit - 7, bufferEndBit)) && bitsNotToEscape == 0)
    {
        uint8_t nextQeuedNibble = (buffer >> bufferEndBit-7) & 0x0F;
        if (nextQeuedNibble == 0x01)
        {
            insertIntoBuffer(command::preserveNextByteFallback, bufferEndBit + 1);
        }
        else
        {
            insertIntoBuffer(command::preserveNextByteDefault, bufferEndBit + 1);
        }

        bufferEndBit += 12;
        bitsNotToEscape = 20;

        if (previousNibble == 0x08)
        {
            negateNibbleInBuffer(bufferEndBit - 3);
        }
    }

    if (bitsNotToEscape > 0)
    {
        bitsNotToEscape -= 4;
    }

    if (hasEqualNibbles(getByteSlice(bufferEndBit - 7, bufferEndBit)))
    {
        negateNibbleInBuffer(bufferEndBit - 3);
        bitsNotToEscape = 4;
    }

    bufferEndBit -= 4;
    uint8_t nextNibble = (buffer >> bufferEndBit+1) & 0x0F;
    previousNibble = nextNibble;
    return nextNibble;
}

std::optional<uint8_t> encoder::nextByte()
{
    uint8_t byte = 0x00;
    byte = (byte | nextNibble().value()) << 4;
    byte |= nextNibble().value();

    return byte;
}