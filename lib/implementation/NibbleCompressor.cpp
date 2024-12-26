#include "../header/NibbleCompressor.hpp"

#include <stdexcept>

NibbleCompressor::NibbleCompressor(const size_t &size)
    : buffer(0x00), dataVector_byte(std::vector<uint8_t>()), nextNibbleIsHighNibble(true)
{
    dataVector_byte.reserve(size);
}

void NibbleCompressor::pushBack(const uint8_t &nibble)
{
    if (nextNibbleIsHighNibble)
    {
        buffer |= nibble << 4;
    }
    else
    {
        buffer |= nibble & 0x0F;
        dataVector_byte.push_back(buffer);
        buffer = 0x00;
    }

    nextNibbleIsHighNibble = !nextNibbleIsHighNibble;
}

std::vector<uint8_t> NibbleCompressor::getData()
{
    if (!nextNibbleIsHighNibble)
    {
#ifndef NDEBUG
        throw std::runtime_error("NibbleCompressor: An odd number of nibbles was provided (last nibble is incomplete). Please ensure that you provide an even number of nibbles for proper compression.");
#endif
    }

    return dataVector_byte;
}
