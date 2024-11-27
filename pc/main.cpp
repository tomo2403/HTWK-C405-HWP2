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

void watchControlPanel()
{
	while (true)
	{
		if (!cp.isConnected()  && cp.isCloseCmdReceived())
			return;

		if (!cp.isConnected())
		{
			// TODO: create handshake packet
			cp.createControlBlock(Flags::CONNECT, 0);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main()
{
	serial.openPort();

	std::vector<uint8_t> inputData = getBinaryInput();
	std::vector<uint8_t> outputData;

	std::thread controlPanelThread(watchControlPanel);
	std::thread receiveThread(processIncomingQueue);
	std::thread sendThread(processOutgoingQueue);

	receiveThread.join();
	sendThread.join();
	controlPanelThread.join();

	serial.closePort();
	setBinaryOutput(outputData);
	return 0;
}
