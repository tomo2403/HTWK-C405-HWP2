#include "../lib/lib.cpp"

#include "Serial.hpp"
#include "DecoderObserver.hpp"

using namespace ioManager;

Serial serial;
DecoderObserver decObserver;
ControlPanel cp;

ThreadSafeQueue<std::vector<uint8_t>> incomingQueue; /**< A queue to store incoming packets. */
ThreadSafeQueue<std::vector<uint8_t>> outgoingQueue; /**< A queue to store outgoing packets. */

void receiveData()
{
	while(cp.isConnected())
	{
		if (!serial.isDataAvailable())
			continue;

		uint8_t byte;
		serial.readByte(byte);

		// TODO: hand over to Decoder
	}
}

void sendData(std::vector<uint8_t> &data)
{
	for (uint8_t byte : data)
	{
		serial.writeByte(byte);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void processIncomingQueue()
{
	while (cp.isConnected())
	{
		if (!incomingQueue.empty())
		{
			std::vector<uint8_t> packet;
			incomingQueue.wait_and_pop(packet);

			// TODO: Process packet
		}
	}
}

void processOutgoingQueue()
{
	while (cp.isConnected())
	{
		if (!outgoingQueue.empty())
		{
			std::vector<uint8_t> packet;
			incomingQueue.wait_and_pop(packet);
			sendData(packet);
		}
	}
}

void watchControlPanel()
{
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
			return;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void connect() {
	std::vector<uint8_t> data = cp.createControlBlock(Flags::CONNECT, 0);
	while (!cp.isConnected())
	{
		sendData(data);
	}
}

int main()
{
	serial.openPort();

	std::vector<uint8_t> inputData = getBinaryInput();
	std::vector<uint8_t> outputData;

	connect();

	std::thread controlPanelThread(watchControlPanel);
	std::thread receiveThread(receiveData);
	std::thread incomingThread(processIncomingQueue);
	std::thread outgoingThread(processOutgoingQueue);

	receiveThread.join();
	incomingThread.join();
	outgoingThread.join();
	controlPanelThread.join();

	serial.closePort();
	setBinaryOutput(outputData);
	return 0;
}
