#include "../header/Decoder.hpp"

Decoder::Decoder(std::vector<uint8_t> &dataVector) : dataVector(dataVector)
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
		case CodecCommand::closeConnection:
			partnerIsReady = false;
			break;
		case CodecCommand::everythingSend:
			everythingReceived = true;
			break;
		case CodecCommand::endBlock:
			packetComplete = true;
			break;
		case CodecCommand::insertEscSeqAsDataDefault:
		case CodecCommand::insertEscSeqAsDataFallback:
			writeToDataVector(escapeSequence);
			break;
		case CodecCommand::insertPrevNibbleAgainDefault:
		case CodecCommand::insertPrevNibbleAgainFallback:
			writeToDataVector(previousNibble);
			break;
		case CodecCommand::beginBlockDefault:
		case CodecCommand::beginBlockFallback:
			dataVector.clear();
			packetComplete = false;
			zeroBuffer();
			dataVectorBuffer = 0x00;
			dataVectorBufferShiftCount = 0;
			bufferEndBit = -1;
			break;
		default:
			break;
	}
}

void Decoder::writeToDataVector(const uint8_t &nibble)
{
	if (!partnerIsReady) return;

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

bool Decoder::hasData() const
{
	return !packetComplete;
}

bool Decoder::connectionIsOnline()
{
	return partnerIsReady;
}

PostPacket Decoder::decodeAll(std::vector<uint8_t> &data)
{
	for (uint8_t nibble: data)
	{
		nextNibble(nibble);
	}
	flushBufferIntoDataVector();

	PostPacket packet;
	packet.index = dataVector[0];
	packet.data = dataVector;

	packet.transferFinished = everythingReceived;
	packet.connectionClosed = !partnerIsReady;

	// last 32 bytes are crc
	uint32_t crc = 0;
	for (int i = 0; i < 4; i++)
	{
		crc <<= 8;
		crc |= dataVector[dataVector.size() - 4 + i];
	}
	packet.crc = crc;
	return packet;
}