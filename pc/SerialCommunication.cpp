#include "SerialCommunication.hpp"

void SerialCommunication::beginBlockReceived(const BlockType &blockType)
{

}

void SerialCommunication::endBlockReceived(const BlockType &blockType)
{
	incomingQueue.push(std::make_pair(blockType, decoder.getDataVector()));
}

void SerialCommunication::receiveData()
{
	while(cp.isConnected())
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
