#include <iostream>
#include <bitset>

#include "../header/Encoder.hpp"
#include "../header/CodecCommand.hpp"

Encoder::Encoder()
{
	initialize();
}

void Encoder::initialize()
{
	Codec::initialize();
	this->dataVectorOffset_Index = 0;
	this->justEscaped = false;
	this->state = unini
	
}

void Encoder::inputData(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
	initialize();
	this->blockType = blockType;
	this->dataVector = dataVector;
}

void Encoder::insertNibbleIntoBuffer(const uint8_t &nibble, const uint8_t &atBit)
{
	if (atBit > 27)
	{
		throw std::out_of_range("The buffer is 32bit in size and the sequence 4bit long.");
	}

	buffer &= ~(0xF << atBit);
	buffer |= (static_cast<uint32_t>(nibble & 0x0F) << atBit);
}

void Encoder::insertByteIntoBuffer(const uint8_t &byte, const uint8_t &atBit)
{
	if (atBit > 24)
	{
		throw std::out_of_range("The buffer is 32bit in size and the sequence 8bit long.");
	}

	buffer &= ~(0xFF << atBit);
	buffer |= (static_cast<uint32_t>(byte) << (atBit));
}

bool Encoder::hasData()
{
	if (dataVectorOffset_Index >= dataVector.size() && bufferEndBit == -1)
	{
		leftShiftNibbleIntoBuffer(escapeSequence);
		leftShiftNibbleIntoBuffer(CodecCommand::endBlock);
		bufferEndBit += 8;
		endBlockWasSent = true;
		justEscaped = true;
	}

	return dataVectorOffset_Index < dataVector.size() || bufferEndBit != -1;
}

uint8_t Encoder::upcomingNibble()
{
	if (bufferEndBit == 3 && dataVectorOffset_Index < dataVector.size())
	{
		return dataVector.at(dataVectorOffset_Index) >> 4;
	}
	else if (bufferEndBit == 3 && dataVectorOffset_Index >= dataVector.size())
	{
		return 0x00; // fishy
	}

	return getNibbleSlice(bufferEndBit - 7);
}

uint8_t Encoder::nextNibble()
{

	if (bufferEndBit < 3 && dataVectorOffset_Index < dataVector.size())
	{
		leftShiftByteIntoBuffer(dataVector.at(dataVectorOffset_Index++));
		bufferEndBit += 8;
	}

	if (!beginBlockWasSent)
	{
		insertNibbleIntoBuffer(escapeSequence, bufferEndBit+5);
		if (currentNibble() == CodecCommand::beginBlockDefault)
		{
			insertNibbleIntoBuffer(CodecCommand::beginBlockFallback, bufferEndBit+1);
		}
		else
		{
			insertNibbleIntoBuffer(CodecCommand::beginBlockDefault, bufferEndBit+1);
		}

		beginBlockWasSent = true;
		bufferEndBit += 8;
		justEscaped = true;
	}

	if (currentNibble() == previousNibble && prevNibbleInitialized)
	{
		const uint8_t upcomingNib = upcomingNibble();
		insertNibbleIntoBuffer(escapeSequence, bufferEndBit-3);

		if (upcomingNib == CodecCommand::insertPrevNibbleAgainDefault)
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertPrevNibbleAgainFallback, bufferEndBit-3);
			evenNumberOfNibblesSent = !evenNumberOfNibblesSent;
		}
		else
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertPrevNibbleAgainDefault, bufferEndBit-3);
			evenNumberOfNibblesSent = !evenNumberOfNibblesSent;
		}
		justEscaped = true;
	}

	if (currentNibble() == escapeSequence && !justEscaped)
	{
		if (upcomingNibble() == CodecCommand::insertEscSeqAsDataDefault)
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertEscSeqAsDataFallback, bufferEndBit-3);
			evenNumberOfNibblesSent = !evenNumberOfNibblesSent;
		}
		else
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertEscSeqAsDataDefault, bufferEndBit-3);
			evenNumberOfNibblesSent = !evenNumberOfNibblesSent;
		}
	}

	justEscaped = false;
	const uint8_t currentNib = currentNibble();
	bufferEndBit -= 4;
	previousNibble = currentNib;
	prevNibbleInitialized = true;
	return currentNib;
}

uint8_t Encoder::nextByte()
{
	uint8_t byte = 0x00;
	byte = (byte | nextNibble()) << 4;
	byte |= nextNibble();

	return byte;
}

std::vector<uint8_t> Encoder::encodeAll()
{
	std::vector<uint8_t> encodedData{};
	while (hasData())
	{
		encodedData.push_back(nextByte());
	}
	return encodedData;
}

std::vector<uint8_t> Encoder::convertPacket(const PrePacket &p)
{
	std::vector<uint8_t> packetData = {p.index};
	packetData.insert(packetData.end(), p.data.begin(), p.data.end());
	// split crc into 4 bytes
	packetData.push_back(p.crc >> 24);
	packetData.push_back(p.crc >> 16);
	packetData.push_back(p.crc >> 8);
	packetData.push_back(p.crc);
	return packetData;
}