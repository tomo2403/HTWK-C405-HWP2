#include <algorithm>

#include "../header/Decoder.hpp"
#include "../header/CodecCommand.hpp"

Decoder::Decoder()
{
	initialize();
}

void Decoder::initialize()
{
	Codec::initialize();
	this->EscapedModeIsActive = false;
	this->dataVectorBuffer = 0x00;
	this->dataVectorBufferShiftCount = 0;
	this->dataVector = std::vector<uint8_t>();
}

void Decoder::addObserver(IDecoderObserver *observer)
{
	observers.push_back(observer);
}

void Decoder::removeObserver(IDecoderObserver *observer)
{
	observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Decoder::processCommand(const uint8_t &command)
{
	switch (command & 0x0F)
	{
	case beginDataBlockDefault:
	case beginDataBlockFallback:
		initialize();
		dataVectorIsLocked = false;
		currentBlockType = dataBlock;
		for (IDecoderObserver *observer : observers)
		{
			observer->beginBlockReceived(dataBlock);
		}
		break;
	case beginControlBlockDefault:
	case beginControlBlockFallback:
		if (!dataVectorIsLocked && !storageHoldsData)
		{
			saveCurrentAttributes();
			initialize();
		}
		dataVectorIsLocked = false;
		currentBlockType = controlBlock;
		storageHoldsData = true;
		for (IDecoderObserver *observer : observers)
		{
			observer->beginBlockReceived(controlBlock);
		}
		break;
	case insertEscSeqAsDataDefault:
	case insertEscSeqAsDataFallback:
		writeToDataVector(escapeSequence);
		break;
	case insertPrevNibbleAgainDefault:
	case insertPrevNibbleAgainFallback:
		writeToDataVector(previousNibble);
		break;
	case endBlockDefault:
	case endBlockFallback:
		flushBufferIntoDataVector();
		for (IDecoderObserver *observer : observers)
		{
			observer->endBlockReceived(currentBlockType, dataVector);
		}

		if (!storageHoldsData)
		{
			dataVectorIsLocked = true;
		}

		restoreSavedAttributes();
		storageHoldsData = false;
	default:
		break;
	}
}

void Decoder::writeToDataVector(const uint8_t &nibble)
{
	if (dataVectorIsLocked) return;
	
	if (dataVectorBufferShiftCount == 2)
	{
		dataVector.push_back(dataVectorBuffer);
		dataVectorBufferShiftCount = 0;
	}

	dataVectorBuffer <<= 4;
	dataVectorBuffer |= nibble & 0x0F;
	dataVectorBufferShiftCount++;
	previousNibble = nibble;
}

void Decoder::flushBufferIntoDataVector()
{
	while (bufferEndBit >= 3)
	{
		writeToDataVector(getNibbleSlice(bufferEndBit - 3));
		bufferEndBit -= 4;
	}

	dataVector.push_back(dataVectorBuffer);

	// TODO: Exception, wenn ein Nibble zu wenig!
}

void Decoder::nextNibble(const uint8_t &nibble)
{
	if (EscapedModeIsActive)
	{
		processCommand(nibble);
		EscapedModeIsActive = false;
		return;
	}

	if (nibble == escapeSequence)
	{
		EscapedModeIsActive = true;
		return;
	}

	writeToDataVector(nibble);
}

void Decoder::saveCurrentAttributes()
{
	this->storage.buffer = this->buffer;
	this->storage.previousNibbleExists = this->previousNibbleExists;
	this->storage.previousNibble = this->previousNibble;
	this->storage.bufferEndBit = this->bufferEndBit;
	this->storage.dataVector = this->dataVector;
	this->storage.EscapedModeIsActive = this->EscapedModeIsActive;
	this->storage.dataVectorBuffer = this->dataVectorBuffer;
	this->storage.dataVectorBufferShiftCount = this->dataVectorBufferShiftCount;
	this->storage.currentBlockType = this->currentBlockType;
}

void Decoder::restoreSavedAttributes()
{
    this->buffer = this->storage.buffer;
    this->previousNibbleExists = this->storage.previousNibbleExists;
    this->previousNibble = this->storage.previousNibble;
    this->bufferEndBit = this->storage.bufferEndBit;
    this->dataVector = this->storage.dataVector;
    this->EscapedModeIsActive = this->storage.EscapedModeIsActive;
    this->dataVectorBuffer = this->storage.dataVectorBuffer;
    this->dataVectorBufferShiftCount = this->storage.dataVectorBufferShiftCount;
    this->currentBlockType = this->storage.currentBlockType;
}
