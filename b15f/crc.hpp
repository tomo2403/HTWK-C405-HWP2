#pragma once
#include <cstdint>
#include <vector>

class CRC
{
public:
    CRC(uint32_t polynomial, uint32_t initialValue = 0x00000000);
    uint32_t calculateCRC(std::vector<uint8_t> data) ;
    bool validateCRC(const std::vector<uint8_t>& data, uint32_t receivedCRC);

private:
    uint32_t polynomial;
    uint32_t initialValue;
};