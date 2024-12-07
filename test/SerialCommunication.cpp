#include "SerialCommunication.hpp"

void SerialCommunication::beginBlockReceived(const BlockType &blockType)
{
    cp.connect();
}

void SerialCommunication::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
	if (dataVector.size() < 7)
		return;
	incomingQueue.push(std::make_pair(blockType, dataVector));
}

void SerialCommunication::receiveData()
{
	while(cp.isConnected() || !cp.isCloseCmdReceived())
	{
		if (!serial.isDataAvailable())
			continue;

		uint8_t byte;
		serial.readByte(byte);
		decoder.nextNibble(byte);
	}
}

SerialCommunication::SerialCommunication(Serial &serial, ControlPanel &cp) : serial(serial), cp(cp)
{
	decoder.addObserver(this);
}
