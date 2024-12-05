#include "../lib/lib.cpp"

#include "Serial.hpp"
#include "SerialCommunication.hpp"

using namespace ioManager;

Serial serial;
ControlPanel cp;
SerialCommunication com(serial, cp);
Encoder encoder;
CRC crc(0x04C11DB7, 0xFFFFFFFF);
uint16_t nextPacketId = 0;

ThreadSafeQueue<std::vector<uint8_t>> outgoingQueue; /**< A queue to store outgoing packets. */

void sendData(BlockType type, std::vector<uint8_t> &data)
{
	encoder.inputDataBlock(data);
	while (encoder.hasData())
	{
		serial.writeByte(encoder.nextNibble());
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void connect()
{
	auto start = std::chrono::steady_clock::now();
	Logger(INFO) << "Connecting...";
	std::vector<uint8_t> data = cp.createControlBlock(Flags::CONNECT, 0);
	crc.attachCRC(data);
	while (!cp.isConnected())
	{
		sendData(BlockType::controlBlock, data);

		auto now = std::chrono::steady_clock::now();
		auto elapsed = duration_cast<std::chrono::seconds>(now - start).count();
		Logger(INFO, true) << "Connecting... " << elapsed << "s elapsed";
		std::this_thread::sleep_for(std::chrono::milliseconds(480));
	}
	Logger(INFO) << "Connected!";
}

void processIncomingQueue(std::vector<uint8_t> &outputData)
{
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
			return;

		if (com.incomingQueue.empty())
			continue;


		std::pair<BlockType, std::vector<uint8_t>> packet;
		com.incomingQueue.wait_and_pop(packet);

		uint32_t id = (packet.second[0] << 8) | packet.second[1];
		std::vector<uint8_t> data(packet.second.begin() + 2, packet.second.end() - 4);
		uint32_t receivedCrc = (packet.second[packet.second.size() - 4] << 24) | (packet.second[packet.second.size() - 3] << 16) |
							   (packet.second[packet.second.size() - 2] << 8) | packet.second[packet.second.size() - 1];
		bool valid = crc.validateCRC({packet.second.begin(), packet.second.end() - 4}, receivedCrc);

		if (packet.first == BlockType::controlBlock)
		{
			if (valid)
				cp.processControlBlock(packet.second[2], id);
		}
		else if (packet.first == BlockType::dataBlock)
		{
			std::vector<uint8_t> controlBlock;
			if (valid)
			{
				outputData.insert(outputData.end(), data.begin(), data.end());
				controlBlock = cp.createControlBlock(Flags::RECEIVED, id);
			}
			else
			{
				controlBlock = cp.createControlBlock(Flags::RESEND, id);
			}
			outgoingQueue.push(controlBlock);
		}
		else
		{
			throw std::invalid_argument("Invalid block type.");
		}
	}
}

void processOutgoingQueue(std::vector<uint8_t> &inputData)
{
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
			return;

		if (!cp.isConnected() && !cp.isCloseCmdReceived())
		{
			connect();
		}
		else if (!outgoingQueue.empty())
		{
			std::vector<uint8_t> packet;
			outgoingQueue.wait_and_pop(packet);
			sendData(BlockType::dataBlock, packet);
		}
		else
		{
			if (!inputData.empty())
			{
				std::vector<uint8_t> data = {static_cast<uint8_t>(nextPacketId >> 8), static_cast<uint8_t>(nextPacketId)};
				nextPacketId++;

				size_t bytesToSend = std::min(static_cast<size_t>(64), inputData.size());
				data.insert(data.end(), inputData.begin(), inputData.begin() + bytesToSend);

				uint32_t crcValue = crc.calculateCRC(data);
				data.push_back(crcValue >> 8);
				data.push_back(crcValue);

				sendData(BlockType::dataBlock, data);

				inputData.erase(inputData.begin(), inputData.begin() + bytesToSend);
			}
			else
			{
				std::vector<uint8_t> data = cp.createControlBlock(Flags::TRANSFER_FINISHED, 0);
				crc.attachCRC(data);
				sendData(BlockType::controlBlock, data);
			}
		}
	}
}

void watchControlPanel()
{
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
		{
			Logger(DEBUG) << "Connection closed!";
			return;
		}

		Logger(INFO, true) << "Current packet id: " << nextPacketId;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void prepareOutgoingQueue(std::vector<uint8_t> &inputData)
{
	Logger(DEBUG) << "Preparing outgoing queue with " << inputData.size() << " bytes...";

	Logger(INFO) << "";
	while (!inputData.empty())
	{
		std::vector<uint8_t> data = {static_cast<uint8_t>(nextPacketId >> 8), static_cast<uint8_t>(nextPacketId)};
		nextPacketId++;

		size_t bytesToSend = std::min(static_cast<size_t>(64), inputData.size());
		data.insert(data.end(), inputData.begin(), inputData.begin() + bytesToSend);

		uint32_t crcValue = crc.calculateCRC(data);
		data.push_back(crcValue >> 8);
		data.push_back(crcValue);

		outgoingDataQueue.push(data);
		inputData.erase(inputData.begin(), inputData.begin() + bytesToSend);
		Logger(INFO, true) << "Data left to process: " << inputData.size() << " bytes";
	}
}

int main()
{
	serial.openPort();

	std::vector<uint8_t> inputData = getBinaryInput();
	std::vector<uint8_t> outputData;

	prepareOutgoingQueue(inputData);

	std::thread receiveThread(&SerialCommunication::receiveData, &com);

	Logger(DEBUG) << "Staring queue threads...";
	std::thread incomingThread(processIncomingQueue, std::ref(outputData));
	std::thread outgoingThread(processOutgoingQueue, std::ref(inputData));

	receiveThread.join();
	incomingThread.join();
	outgoingThread.join();

	Logger(DEBUG) << "Queues stopped.";

	serial.closePort();
	setBinaryOutput(outputData);
	return 0;
}
