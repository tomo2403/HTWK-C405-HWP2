#pragma once

#include "Codec.hpp"
#include "BlockType.hpp"

class Encoder : public Codec
{
private:

	// state Pattern?
	enum State
	{
		UNINITIALIZED,
		START,
		DATA,
		END,
	};

	std::vector<uint8_t> dataVector;
	uint32_t dataVectorOffset_Index;
	bool justEscaped;
	BlockType blockType;
	State state;

	void initialize();
	
	void insertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &atBit);

	void insertByteIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

	uint8_t upcomingNibble();

public:
	Encoder();

	void inputData(const BlockType &blockType, const std::vector<uint8_t> &dataVector);

	bool hasData();

	uint8_t nextByte();

	uint8_t nextNibble();

	std::vector<uint8_t> encodeAll();
};