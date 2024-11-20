#include <iostream>
#include <bitset>

#include "Encoder.hpp"

Encoder::Encoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector) : Codec::Codec(escapeSequence, dataVector)
{
	if (dataVector.empty())
	{
		throw std::invalid_argument("Data-Vector cannot be empty.");
	}

	clock = 0;
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
	return !(stopSoon && bufferEndBit == -1) || !stopSoon;
}

uint8_t Encoder::getNewRawNibble()
{
	if(escScannerNibbleCount < 2 && dataVectorOffset_Index < dataVector.size())
	{	
		uint8_t newByte = dataVector.at(dataVectorOffset_Index++);
		escScanningBuffer <<= 8;
		escScanningBuffer |= newByte;
		escScannerNibbleCount += 2;
	} else if (escScannerNibbleCount < 2 && !dataVectorOffset_Index < dataVector.size())
	{
		escScanningBufferHasData = false;
		return escScanningBuffer & 0x0F;
	}

	const uint8_t upcommingByte = escScanningBuffer >> (escScannerNibbleCount - 2);
	
	if (upcommingByte == escapeSequence && escScannerNibbleNotToEscape == 0)
	{
		if (escScannerNibbleCount == 2)
		{
			escScanningBuffer <<= 4;
			escScanningBuffer |= CodecCommand::insertEscSeqAsDataDefault;
		}
		else if (escScannerNibbleCount == 3)
		{
			uint16_t escScanningBufferTmp = escScanningBuffer;
			escScanningBuffer &= 0x000F;
			escScanningBuffer |= CodecCommand::insertEscSeqAsDataDefault;
			escScanningBuffer <<= 4;
			escScanningBuffer |= escScanningBufferTmp & 0x000F;
		}
		escScannerNibbleCount += 1;
		escScannerNibbleNotToEscape += 3;
	}

	escScannerNibbleCount--;
	uint8_t ret = (escScanningBuffer >> (escScannerNibbleCount*4)) & 0x0F;
	return ret;
}

uint8_t Encoder::currentThreeBit()
{
	return static_cast<uint8_t>((buffer >> (bufferEndBit-3)) & 0x00000007);
}

uint8_t Encoder::nextNibble()
{
	if (bufferEndBit < 2)
	{
		if(escScanningBufferHasData)
		{
			leftShiftNibbleIntoBuffer(getNewRawNibble());
			bufferEndBit += 4;
		}
		else if (bufferEndBit == -1)
		{
			leftShiftByteIntoBuffer(escapeSequence);
			leftShiftNibbleIntoBuffer(CodecCommand::STOP);
			bufferEndBit = 11;
			stopSoon = true;
		}
		else if (bufferEndBit == 0)
		{
			const uint8_t remainingBit = static_cast<uint8_t>(buffer & 0x1);
			leftShiftByteIntoBuffer(escapeSequence);
			leftShiftNibbleIntoBuffer(CodecCommand::onlyReadOneBitOfNextNibble);
			buffer = (buffer << 3) | remainingBit;
			bufferEndBit = 14;
		}
		else if (bufferEndBit == 1)
		{
			const uint8_t remainingTwoBits = static_cast<uint8_t>(buffer & 0x3);
			leftShiftByteIntoBuffer(escapeSequence);
			leftShiftNibbleIntoBuffer(CodecCommand::onlyReadTwoBitsOfNextNibble);
			buffer = (buffer << 3) | remainingTwoBits;
			bufferEndBit = 14;
		}
	}

	const uint8_t ret = static_cast<uint8_t>((buffer >> (bufferEndBit-2)) & 0x00000007);
	bufferEndBit -= 3;
	return ret;
}

uint8_t Encoder::nextByte()
{
	uint8_t byte = 0x00;
	byte = (byte | nextNibble()) << 4;
	byte |= nextNibble();

	return byte;
}