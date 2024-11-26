#include "../lib/lib.cpp"

#include "Serial.hpp"
#include "DecoderObserver.hpp"

using namespace ioManager;

Serial serial;
DecoderObserver decObserver;
ControlPanel cp;

ThreadSafeQueue<std::vector<uint8_t>> incomingQueue; /**< A queue to store incoming packets. */
ThreadSafeQueue<std::vector<uint8_t>> outgoingQueue; /**< A queue to store outgoing packets. */

void receiveData(std::vector<uint8_t> &data)
{
	while (serial.isDataAvailable())
	{
		uint8_t byte;
		serial.readByte(byte);

		// TODO: Decode and stop receiving data.
		//data.push_back(byte);
	}
}

void processIncomingQueue()
{
	while (cp.isConnected())
	{
		if (!incomingQueue.empty())
		{

		}
	}
}

void processOutgoingQueue()
{
	while (cp.isConnected())
	{
		if (!outgoingQueue.empty())
		{

		}
	}
}

int main()
{
	serial.openPort();

	std::vector<uint8_t> inputData = getBinaryInput();
	std::vector<uint8_t> outputData;

	// TODO: handshake

	std::thread receiveThread(processIncomingQueue);
	std::thread sendThread(processOutgoingQueue);

	receiveThread.join();
	sendThread.join();

	serial.closePort();
	setBinaryOutput(outputData);
	return 0;
}
