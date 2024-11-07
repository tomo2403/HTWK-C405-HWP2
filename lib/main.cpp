#include "encoder.hpp"
#include <iostream>
#include <bitset>

int main(int, char**){
    
    std::vector<uint8_t> bytes = {0x02, 0x89, 0xE1};
    encoder enc = encoder(0x80, bytes);

    while (enc.hasData())
    {
        std::cout << std::bitset<8>(enc.nextByte().value()) << std::endl;
    }
}