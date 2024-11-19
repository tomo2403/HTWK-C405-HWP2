#include <iostream>
#include <bitset>

#include "Encoder.hpp"

Encoder::Encoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector) : Codec::Codec(escapeSequence, dataVector)
{
	if (dataVector.empty())
	{
		throw std::invalid_argument("Data-Vector cannot be empty.");
	}
	else
	{
		leftShiftByteIntoBuffer(dataVector.at(dataVectorOffset_Index++));
		bufferEndBit = 7;
	}
}

void Encoder::insertIntoBuffer(const command &command, const uint8_t &atBit)
{
	if (atBit > 20)
	{
		throw std::out_of_range("The buffer is 32bit in size and the sequence 12bit long.");
	}

	buffer &= ~(0xFFF << atBit);
	buffer |= (static_cast<uint32_t>(escapeSequence) << (atBit + 4));
	buffer |= (static_cast<uint32_t>(command) << atBit);
}

void Encoder::insertIntoBuffer(const uint8_t &byte, const uint8_t &atBit)
{
	if (atBit < 0 || atBit > 24)
	{
		throw std::out_of_range("The buffer is 32bit in size and the sequence 8bit long.");
	}

	buffer &= ~(0xFF << atBit);
	buffer |= (static_cast<uint32_t>(byte) << (atBit));
}

bool Encoder::hasData()
{
	return bufferEndBit != 0 || dataVectorOffset_Index < dataVector.size();
}

bool Encoder::fetchDataIfBufferTooSmall()
{
	const bool bufferContainsOnlyOneNibble = bufferEndBit <= 3;
	const bool dataVectorHasUnprocessedData = dataVectorOffset_Index < dataVector.size();

	if (bufferContainsOnlyOneNibble)
	{
		if (!dataVectorHasUnprocessedData)
		{
			return false;
		}

		leftShiftByteIntoBuffer(dataVector.at(dataVectorOffset_Index++));
		bufferEndBit += 8;
	}

	return true;
}

uint8_t Encoder::upcommingByte()
{
	if (bufferEndBit >= 11)
	{
		return static_cast<uint8_t>((buffer >> (bufferEndBit - 11)) & 0x000000FF);
	}
	else if (dataVectorOffset_Index < dataVector.size())
	{
		return (upcommingNibble() << 4)| (dataVector.at(dataVectorOffset_Index) >> 4);
	}
	else
	{
		throw std::out_of_range("No more Data in Datastream.");
	}
}

std::optional<uint8_t> Encoder::nextNibble()
{
	if (!fetchDataIfBufferTooSmall())
	{
		bufferEndBit = 0;
		return static_cast<uint8_t>(buffer & 0x0F);
	}

	if (currentByte() == escapeSequence && bitsNotToEscape == 0)
	{
		insertIntoBuffer(command::preserveNextByteDefault, bufferEndBit + 1);
		bufferEndBit += 12;
		bitsNotToEscape = 20;
	}

	if (currentNibble() == (escapeSequence >> 4))
	{	
		if (hasNegatedNibbles(upcommingByte()))
		{
			negateNibbleInBuffer(bufferEndBit - 3);
		}

		if (upcommingByte() == 0xFF)
		{
			negateNibbleInBuffer(bufferEndBit - 7);	
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertEscSeqAsDataDefault & 0x0F, bufferEndBit-3);
		}
	}

	if (areNegated(currentNibble(), upcommingNibble()) && bitsNotToEscape == 0)
	{
		if (currentNibble() == 0x01)
		{
			insertIntoBuffer(command::preserveNextByteFallback, bufferEndBit + 1);
		}
		else
		{
			insertIntoBuffer(command::preserveNextByteDefault, bufferEndBit + 1);
		}

		bufferEndBit += 12;
		bitsNotToEscape = 20;

		if (previousNibble == 0x08)
		{
			negateNibbleInBuffer(bufferEndBit - 3);
		}
	}

	if (bitsNotToEscape > 0)
	{
		bitsNotToEscape -= 4;
	}

	if (hasEqualNibbles(getByteSlice(bufferEndBit - 7)) && previousNibble != ((~getNibbleSlice(bufferEndBit - 3)) & 0x0F))
	{
		negateNibbleInBuffer(bufferEndBit - 3);
		bitsNotToEscape = 4;
	}

	bufferEndBit -= 4;
	uint8_t nextNibble = (buffer >> (bufferEndBit + 1)) & 0x0F;
	previousNibble = nextNibble;
	return nextNibble;
}

std::optional<uint8_t> Encoder::nextByte()
{
	uint8_t byte = 0x00;
	byte = (byte | nextNibble().value()) << 4;
	byte |= nextNibble().value();

	return byte;
}