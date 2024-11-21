#pragma once

#include <optional>

#include "Codec.hpp"

class Encoder : public Codec
{
private:
	uint32_t dataVectorOffset_Index = 0;
	bool justEscaped = false;
	
	void insertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &atBit);

	void insertByteIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

public:

	Encoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector);

	bool hasData();

	uint8_t nextByte();

	uint8_t nextNibble();

	std::vector<uint8_t> encodeAll();
};