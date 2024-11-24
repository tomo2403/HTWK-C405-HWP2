#pragma once

#include <optional>

#include "Codec.hpp"

class Encoder : public Codec
{
private:
	uint32_t dataVectorOffset_Index = 0;
	bool justEscaped = false;
	bool prevNibbleInitialized = false;
	bool evenNumberOfNibblesSent = true;
	bool beginBlockWasSent = false;
	bool endBlockWasSent = false;
	
	void insertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &atBit);

	void insertByteIntoBuffer(const uint8_t &byte, const uint8_t &atBit);

	uint8_t upcomingNibble();
	std::vector<uint8_t> dataVector;

public:
	explicit Encoder(const std::vector<uint8_t>& dataVector);

	static std::vector<uint8_t> convertPacket(const PrePacket &p);

	bool hasData();

	uint8_t nextByte();

	uint8_t nextNibble();

	std::vector<uint8_t> encodeAll();

	void newDataVector(const std::vector<uint8_t>& data);
};