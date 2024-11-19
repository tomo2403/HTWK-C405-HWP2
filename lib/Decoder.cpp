#include "Decoder.hpp"

Decoder::Decoder(uint8_t escapeSequence, std::vector<uint8_t> &dataVector) : Codec::Codec(escapeSequence, dataVector)
{
	this->bufferEndBit = 0;
}

void Decoder::processCommand(const uint8_t &command)
{
	switch (command & 0x0F)
	{
		case CodecCommand::preserveNextByteDefault:
			nibblesNotToDecode = 2;
			break;
		case CodecCommand::preserveNextByteFallback:
			nibblesNotToDecode = 2;
			break;
		case CodecCommand::iAmReady:
			partnerIsReady = true;
			break;
		case CodecCommand::insertEscSeqAsDataDefault:
			leftShiftByteIntoBuffer(0x80);
			bufferEndBit = bufferEndBit == 0 ? 7 : bufferEndBit+8;
			// TODO decoding muss 1x mehr laufen, aber ohne neues Nibble
			escAllowed = false;
			break;
		default:
			break;
	}
}

void Decoder::writeToDataVector(const uint8_t &nibble)
{
	// if (!partnerIsReady) return;

	dataVectorBuffer <<= 4;
	dataVectorBuffer |= nibble & 0x0F;
	dataVectorBufferShiftCount++;

	if (dataVectorBufferShiftCount == 2)
	{
		dataVector.push_back(dataVectorBuffer);
		dataVectorBufferShiftCount = 0;
	}
}

void Decoder::flushBufferIntoDataVector()
{
	while (bufferEndBit > 0)
	{
		writeToDataVector(getNibbleSlice(bufferEndBit - 3));
		bufferEndBit = bufferEndBit == 3 ? 0 : bufferEndBit - 4;
	}

	// TODO: Exception, wenn ein Nibble zu wenig!
}

void Decoder::nextNibble(const uint8_t &nibble)
{
	if (nibblesNotToDecode > 0)
	{
		writeToDataVector(nibble);
		nibblesNotToDecode--;
		flippedPevNibble = false;
		previousNibble = nibble;
		return;
	}

	if (EscapedModeIsActive)
	{
		processCommand(nibble);
		EscapedModeIsActive = false;
		return;
	}


	leftShiftNibbleIntoBuffer(nibble);
	bufferEndBit = bufferEndBit == 0 ? 3 : bufferEndBit + 4;

	// Es gibt nichts zu überprüfen
	if (bufferEndBit < 7)
	{
		return;
	}

	const uint8_t currentByte = getByteSlice(bufferEndBit - 7);
	const uint8_t testWTF = ((~currentByte) >> 4) & 0x0F;
	const bool test = previousNibble == testWTF;

	if ((hasNegatedNibbles(currentByte) || test) && !flippedPevNibble)
	{
		negateNibbleInBuffer(bufferEndBit - 3);
		flippedPevNibble = true;
	}
	else
	{
		flippedPevNibble = false;
	}

	if (getByteSlice(bufferEndBit - 7) == escapeSequence && escAllowed)
	{
		EscapedModeIsActive = true;
		bufferEndBit -= 7;
		return;
	}

	escAllowed = true;
	previousNibble = getNibbleSlice(bufferEndBit - 3);
	writeToDataVector(getNibbleSlice(bufferEndBit - 3));
	bufferEndBit -= 4;
	timesToRun--;
};