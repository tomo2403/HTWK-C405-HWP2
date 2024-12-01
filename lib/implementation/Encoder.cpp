#include <iostream>
#include <bitset>

#include "../header/Encoder.hpp"
#include "../header/CodecCommand.hpp"

Encoder::Encoder()
{
	initialize();
	this->storageHoldsData = false;
	this->storage = Storage();
}

void Encoder::initialize()
{
	Codec::initialize();
	this->dataVectorOffset_Index = 0;
	this->justEscaped = false;
	this->endBlockWasSent = false;
}

void Encoder::insertStartBlockIntoBuffer()
{
	leftShiftNibbleIntoBuffer(CodecCommand::escapeSequence);

	if (blockType == controlBlock && upcomingNibble() != CodecCommand::beginControlBlockDefault)
	{
		leftShiftNibbleIntoBuffer(CodecCommand::beginControlBlockDefault);
	}
	else if (blockType == controlBlock && upcomingNibble() == CodecCommand::beginControlBlockDefault)
	{
		leftShiftNibbleIntoBuffer(CodecCommand::beginControlBlockFallback);
	}
	else if (blockType == dataBlock && upcomingNibble() != CodecCommand::beginDataBlockDefault)
	{
		leftShiftNibbleIntoBuffer(CodecCommand::beginDataBlockDefault);
	}
	else if (blockType == dataBlock && upcomingNibble() == CodecCommand::beginDataBlockDefault)
	{
		leftShiftNibbleIntoBuffer(CodecCommand::beginDataBlockFallback);
	}

	bufferEndBit += 8;
	justEscaped = true;
}

void Encoder::inputDataBlock(const std::vector<uint8_t> &dataVector)
{
	initialize();
	this->blockType = dataBlock;
	this->dataVector = dataVector;
	insertStartBlockIntoBuffer();
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
	if (dataVectorOffset_Index >= dataVector.size() && bufferEndBit == -1 && !endBlockWasSent)
	{
		leftShiftNibbleIntoBuffer(escapeSequence);
		// TODO: Implement Fallback sequence
		leftShiftNibbleIntoBuffer(CodecCommand::endBlockDefault);
		bufferEndBit += 8;
		justEscaped = true;
		endBlockWasSent = true;
	} else if (dataVectorOffset_Index >= dataVector.size() && bufferEndBit == -1 && endBlockWasSent && storageHoldsData)
	{
		restoreSavedAttributes();
		storageHoldsData = false;
	}

	return dataVectorOffset_Index < dataVector.size() || bufferEndBit != -1;
}

uint8_t Encoder::upcomingNibble()
{
	if (bufferEndBit < 7 && dataVectorOffset_Index < dataVector.size())
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

	if (currentNibble() == previousNibble && previousNibbleExists)
	{
		const uint8_t upcomingNib = upcomingNibble();
		insertNibbleIntoBuffer(escapeSequence, bufferEndBit-3);

		if (upcomingNib == CodecCommand::insertPrevNibbleAgainDefault)
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertPrevNibbleAgainFallback, bufferEndBit-3);
		}
		else
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertPrevNibbleAgainDefault, bufferEndBit-3);
		}
		justEscaped = true;
	}

	if (currentNibble() == escapeSequence && !justEscaped)
	{
		if (upcomingNibble() == CodecCommand::insertEscSeqAsDataDefault)
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertEscSeqAsDataFallback, bufferEndBit-3);
		}
		else
		{
			gracefullyInsertNibbleIntoBuffer(CodecCommand::insertEscSeqAsDataDefault, bufferEndBit-3);
		}
	}

	justEscaped = false;
	const uint8_t currentNib = currentNibble();
	bufferEndBit -= 4;
	previousNibble = currentNib;
	previousNibbleExists = true;
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

void Encoder::interruptWithControlBlock(const std::vector<uint8_t> &controlVector)
{
	saveCurrentAttributes();
	initialize();
	this->blockType = controlBlock;
	this->dataVector = controlVector;
	this->storageHoldsData = true;
	insertStartBlockIntoBuffer();
}

void Encoder::saveCurrentAttributes()
{
	this->storage.buffer = this->buffer;
	this->storage.previousNibbleExists = this->previousNibbleExists;
	this->storage.previousNibble = this->previousNibble;
	this->storage.bufferEndBit = this->bufferEndBit;
	this->storage.dataVector = this->dataVector;
	this->storage.dataVectorOffset_Index = this->dataVectorOffset_Index;
	this->storage.justEscaped = this->justEscaped;
	this->storage.blockType = this->blockType;
	this->storage.endBlockWasSent = this->endBlockWasSent;
}

void Encoder::restoreSavedAttributes()
{
    this->buffer = this->storage.buffer;
    this->previousNibbleExists = this->storage.previousNibbleExists;
    this->previousNibble = this->storage.previousNibble;
    this->bufferEndBit = this->storage.bufferEndBit;
    this->dataVector = this->storage.dataVector;
    this->dataVectorOffset_Index = this->storage.dataVectorOffset_Index;
    this->justEscaped = this->storage.justEscaped;
    this->blockType = this->storage.blockType;
    this->endBlockWasSent = this->storage.endBlockWasSent;
}
