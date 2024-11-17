#include "encoder.hpp"
#include "decoder.hpp"
#include <iostream>
#include <bitset>

int main(int, char**){
    
    std::vector<uint8_t> RawBytes = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE};
    std::vector<uint8_t> decodedBytes = std::vector<uint8_t>();
    encoder enc = encoder(0x80, RawBytes);
    decoder dec = decoder(0x80, decodedBytes);

    std::cout << "Encoded:" << std::endl;
    while (enc.hasData())
    {
        uint8_t nextNibble = enc.nextNibble().value();
        std::cout << std::bitset<8>(nextNibble) << std::endl;
        dec.nextNibble(nextNibble);
    }
    
    std::cout << "Decoded:" << std::endl;
    dec.flushBufferIntoDataVector();
    for (u_int8_t byte : decodedBytes)
    {
        std::cout << std::bitset<8>(byte) << std::endl;
    }
}