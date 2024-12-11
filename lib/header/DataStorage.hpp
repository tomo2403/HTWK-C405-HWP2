#pragma once

#include <vector>
#include <cstdint>

class DataStorage
{
private:
    std::vector<uint8_t> dataVector_byte;
    uint32_t dataVectorOffset;
    bool nextNibbleIsHighNibble;
    
public:
    // TODO: Remove default constructor.
    DataStorage();
    DataStorage(const std::vector<uint8_t> dataVector_byte);

    uint8_t peek_nibble();
    uint8_t pop_nibble();
    bool empty();
};