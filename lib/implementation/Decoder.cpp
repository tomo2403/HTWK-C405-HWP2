#include "../header/Decoder.hpp"

Decoder::Decoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector) : Codec::Codec(escapeSequence, dataVector)
{
	this->bufferEndBit = 0;
}

void Decoder::processCommand(const uint8_t &command)
{
	switch (command & 0x0F)
	{
		case CodecCommand::iAmReady:
			partnerIsReady = true;
			break;
		case CodecCommand::STOP:
			everythingReceived = true;
			break;
		case CodecCommand::insertEscSeqAsDataDefault:
		case CodecCommand::insertEscSeqAsDataFallback:
			writeToDataVector(escapeSequence);
			break;
		case CodecCommand::insertPrevNibbleAgainDefault:
		case CodecCommand::insertPrevNibbleAgainFallback:
			writeToDataVector(previousNibble);
		default:
			break;
	}
}

void Decoder::writeToDataVector(const uint8_t &nibble)
{
	// if (!partnerIsReady) return;

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
	if(EscapedModeIsActive)
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

bool Decoder::hasData() const
{
	return !everythingReceived;
}

bool Decoder::connectionIsOnline() const
{
	return partnerIsReady;
};