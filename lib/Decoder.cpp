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
		case CodecCommand::preserveNextByteFallback:
			nibblesNotToDecode = 2;
			nibbleNotToFlip = 3;
			break;
		case CodecCommand::iAmReady:
			partnerIsReady = true;
			break;
		case CodecCommand::insertEscSeqAsDataDefault:
			leftShiftByteIntoBuffer(0x80);
			bufferEndBit = bufferEndBit == 0 ? 7 : bufferEndBit+8;
			timesToRun += 2;
			escAllowed = false;
			break;
		case CodecCommand::unflipPrevNibbleAndPreserveNextByteDefault:
		case CodecCommand::unflipPrevNibbleAndPreserveNextByteFallback:
			dataVectorBuffer = (dataVectorBuffer & 0xF0) | (~dataVectorBuffer & 0x0F);
			nibblesNotToDecode = 2;
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

	if (nibble == 0x08 && waitForEsc)
	{
		timesToRun += 2;
		waitForEsc = false;
		return;
	}

	do
	{
		// Es gibt nichts zu überprüfen
		if (bufferEndBit < 7)
		{
			return;
		}

		const uint8_t currentByte = getByteSlice(bufferEndBit - 7);

		if (getByteSlice(bufferEndBit - 7) == escapeSequence && escAllowed)
		{
			EscapedModeIsActive = true;
			bufferEndBit = bufferEndBit == 7 ? 0 : bufferEndBit-8;
			return;
		}

		if ((hasNegatedNibbles(currentByte)) && !flippedPevNibble && nibbleNotToFlip == 0)
		{
			negateNibbleInBuffer(bufferEndBit - 3);
			flippedPevNibble = true;
		}
		else
		{
			flippedPevNibble = false;
		}

		escAllowed = true;
		previousNibble = getNibbleSlice(bufferEndBit - 3);
		writeToDataVector(getNibbleSlice(bufferEndBit - 3));
		bufferEndBit -= 4;
		timesToRun--;
		nibbleNotToFlip = nibbleNotToFlip == 0 ? 0 : nibbleNotToFlip-1;
	} while(timesToRun > 0);
	waitForEsc = true;
};