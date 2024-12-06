#pragma once
#include <cstdint>
#include <iomanip>
#include <vector>
#include <iostream>

class B15Fake
{
private:
    std::vector<uint8_t> readData;
    int readDataOffset = 0;

public:
    B15Fake(const std::vector<uint8_t> &readData) : readData(readData)
    {}

    uint8_t getRegister(volatile uint8_t *adr)
    {
        uint8_t value = readData.at(readDataOffset++);
        std::cout << "getReg: " << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(value) << std::endl;
        return value;
    };

    void setRegister(volatile uint8_t *adr, uint8_t val)
    {
        std::cout << "setReg: " << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(val) << std::endl;
        return;
    };
};