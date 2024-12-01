#pragma once

#include "Codec.hpp"
#include "BlockType.hpp"

class Encoder : public Codec
{
private:
	struct Storage
	{
		uint32_t buffer;
		bool previousNibbleExists;
		uint8_t previousNibble;
		int8_t bufferEndBit;
		std::vector<uint8_t> dataVector;
		uint32_t dataVectorOffset_Index;
		bool justEscaped;
		BlockType blockType;
		bool endBlockWasSent;
	};

	std::vector<uint8_t> dataVector;
	uint32_t dataVectorOffset_Index;
	bool justEscaped;
	BlockType blockType;
	bool endBlockWasSent;

	bool controlBlockIsQueued = false;
	bool storageHoldsData;
	Storage storage;

	void initialize();
	
	void insertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &atBit);

	void insertByteIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

	uint8_t upcomingNibble();

	void insertStartBlockIntoBuffer();

	void saveCurrentAttributes();

	void restoreSavedAttributes();

	uint8_t upcomingNibbleFromStorage();

public:
	Encoder();

	void inputDataBlock(const std::vector<uint8_t> &dataVector);

	void interruptWithControlBlock(const std::vector<uint8_t> &controlVector);

	bool hasData();

	uint8_t nextByte();

	uint8_t nextNibble();

	std::vector<uint8_t> encodeAll();
};