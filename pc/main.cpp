#include "../lib/lib.cpp"

#include "Serial.hpp"
#include "SerialCommunication.hpp"

using namespace ioManager;

Serial serial;
ControlPanel cp;
SerialCommunication com(serial, cp);
Encoder encoder;
CRC crc(0x04C11DB7, 0xFFFFFFFF);
uint32_t nextPacketId = 0;

ThreadSafeQueue<std::vector<uint8_t>> outgoingQueue; /**< A queue to store outgoing packets. */

void sendData(BlockType type, std::vector<uint8_t> &data)
{
	encoder.inputData(type, data);
	while (encoder.hasData())
	{
		serial.writeByte(encoder.nextNibble());
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void processIncomingQueue(std::vector<uint8_t> &outputData)
{
	while (cp.isConnected())
	{
		if (!com.incomingQueue.empty())
		{
			std::pair<BlockType, std::vector<uint8_t>> packet;
			com.incomingQueue.wait_and_pop(packet);

			if (packet.first == BlockType::controlBlock)
			{
				cp.processControlBlock(packet.second[0],
									   packet.second[1] << 24 | packet.second[2] << 16 | packet.second[3] << 8 | packet.second[4]);
			}
			else if (packet.first == BlockType::dataBlock)
			{
				std::vector<uint8_t> data(packet.second.begin(), packet.second.end() - 4);
				uint32_t receivedCrc = packet.second[packet.second.size() - 4] << 24 | packet.second[packet.second.size() - 3] << 16 |
									   packet.second[packet.second.size() - 2] << 8 | packet.second[packet.second.size() - 1];
				uint32_t id = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];

				if (crc.validateCRC(data, receivedCrc))
				{
					outputData.insert(outputData.end(), data.begin() + 4, data.end());
					std::vector<uint8_t> controlBlock = cp.createControlBlock(Flags::RECEIVED, id);
					outgoingQueue.push(controlBlock);
				}
				else
				{
					std::vector<uint8_t> controlBlock = cp.createControlBlock(Flags::RESEND, id);
					outgoingQueue.push(controlBlock);
				}
			}
			else
			{
				throw std::invalid_argument("Invalid block type.");
			}
		}
	}
}

void processOutgoingQueue(std::vector<uint8_t> &inputData)
{
	while (cp.isConnected())
	{
		if (!outgoingQueue.empty())
		{
			std::vector<uint8_t> packet;
			outgoingQueue.wait_and_pop(packet);
			sendData(BlockType::dataBlock, packet);
		}
		else
		{
			if (!inputData.empty())
			{
				std::vector<uint8_t> data;
				data.push_back(nextPacketId >> 24);
				data.push_back(nextPacketId >> 16);
				data.push_back(nextPacketId >> 8);
				data.push_back(nextPacketId);
				nextPacketId++;

				size_t bytesToSend = std::min(static_cast<size_t>(64), inputData.size());
				data.insert(data.end(), inputData.begin(), inputData.begin() + bytesToSend);

				uint32_t crcValue = crc.calculateCRC(data);
				data.push_back(crcValue >> 24);
				data.push_back(crcValue >> 16);
				data.push_back(crcValue >> 8);
				data.push_back(crcValue);

				sendData(BlockType::dataBlock, data);

				inputData.erase(inputData.begin(), inputData.begin() + bytesToSend);
			}
			else {
				std::vector<uint8_t> data = cp.createControlBlock(Flags::TRANSFER_FINISHED, 0);
				uint32_t crcValue = crc.calculateCRC(data);
				data.push_back(crcValue >> 24);
				data.push_back(crcValue >> 16);
				data.push_back(crcValue >> 8);
				data.push_back(crcValue);
				sendData(BlockType::controlBlock, data);
			}
		}
	}
}

void connect()
{
	std::vector<uint8_t> data = cp.createControlBlock(Flags::CONNECT, 0);
	while (!cp.isConnected())
	{
		sendData(BlockType::controlBlock, data);
	}
}

int main()
{
	serial.openPort();

	std::vector<uint8_t> inputData = getBinaryInput();
	std::vector<uint8_t> outputData;

	std::thread receiveThread(&SerialCommunication::receiveData, &com);

	connect();

	std::thread incomingThread(processIncomingQueue, std::ref(outputData));
	std::thread outgoingThread(processOutgoingQueue, std::ref(inputData));

	receiveThread.join();
	incomingThread.join();
	outgoingThread.join();

	serial.closePort();
	setBinaryOutput(outputData);
	return 0;
}
