#include <iostream>
#include <bitset>

#include "Encoder.hpp"

Encoder::Encoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector) : Codec::Codec(escapeSequence, dataVector)
{
	if (dataVector.empty())
	{
		throw std::invalid_argument("Data-Vector cannot be empty.");
	}
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
	return dataVectorOffset_Index < dataVector.size() || bufferEndBit != -1;
}

uint8_t Encoder::nextNibble()
{
	if (bufferEndBit < 3 && dataVectorOffset_Index < dataVector.size())
	{
		leftShiftByteIntoBuffer(dataVector.at(dataVectorOffset_Index++));
		bufferEndBit += 8;
	}

	if (currentNibble() == previousNibble)
	{
		insertNibbleIntoBuffer(escapeSequence, bufferEndBit-3); // replace current nibble
		gracefullyInsertNibbleIntoBuffer(CodecCommand::insertPrevNibbleAgain, bufferEndBit-3); // insert cmf after current nibble
		justEscaped = true;
	}

	if (currentNibble() == escapeSequence && !justEscaped)
	{
		gracefullyInsertNibbleIntoBuffer(CodecCommand::insertEscSeqAsData, bufferEndBit-3);
	}

	justEscaped = false;
	const uint8_t currentNib = currentNibble();
	bufferEndBit -= 4;
	previousNibble = currentNib;
	return currentNib;
}

uint8_t Encoder::nextByte()
{
	uint8_t byte = 0x00;
	byte = (byte | nextNibble()) << 4;
	byte |= nextNibble();

	return byte;
}