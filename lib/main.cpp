#include "encoder.hpp"
#include <iostream>
#include <bitset>

int main(int, char**){
    
    std::vector<uint8_t> bytes = {0x80, 0xE9, 0x01};
    encoder enc = encoder(0x80, bytes);

    while (enc.hasData())
    {
        std::cout << std::bitset<8>(enc.nextByte().value()) << std::endl;
    }
}