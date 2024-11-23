#include "header/Encoder.hpp"
#include "header/Decoder.hpp"
#include <bitset>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>



int main(int, char**){
    
    std::vector<uint8_t> RawBytes = {0x98, 0x0e, 0x26, 0xff};
    std::vector<uint8_t> decodedBytes = std::vector<uint8_t>();
    Encoder enc = Encoder(RawBytes);
    Decoder dec = Decoder(decodedBytes);

	std::cout << "Encoded:" << std::endl;
	while (enc.hasData())
	{
		uint8_t nextNibble = enc.nextNibble();
		std::cout << std::bitset<8>(nextNibble) << std::endl;
		dec.nextNibble(nextNibble);
	}

	std::cout << "Decoded:" << std::endl;
	dec.flushBufferIntoDataVector();
	for (uint8_t byte: decodedBytes)
	{
		std::cout << std::bitset<8>(byte) << std::endl;
	}
}