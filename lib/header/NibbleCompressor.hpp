#pragma once

#include <vector>
#include <cstdint>

class NibbleCompressor
{
    uint8_t buffer;
    std::vector<uint8_t> dataVector_byte;
    bool nextNibbleIsHighNibble;

public:
    explicit NibbleCompressor(const std::size_t &size = 0);

    void pushBack(const uint8_t &nibble);

    std::vector<uint8_t> getData();
};
