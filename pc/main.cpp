#include "../lib/lib.cpp"
#include <condition_variable>
#include <mutex>

#include "Serial.hpp"
#include "SerialCommunication.hpp"

using namespace ioManager;

Serial serial;
ControlPanel cp;
SerialCommunication com(serial, cp);
Encoder encoder;
CRC crc(0x04C11DB7, 0xFFFFFFFF);
uint16_t nextPacketId = 0;
u_long errors = 0;

std::vector<std::vector<uint8_t>> outgoingData{};
ThreadSafeQueue<std::vector<uint8_t>> outgoingResponseQueue;

std::mutex mtx;
std::condition_variable cv;
bool responsePending = false;

std::thread sendDataThread;
std::thread sendResponseThread;

void sendData(std::vector<uint8_t> &data)
{
	encoder.inputDataBlock(data);
	while (encoder.hasData())
	{
		std::unique_lock<std::mutex> lock(mtx);
		while (responsePending)
		{
			cv.wait(lock);
		}

		serial.writeByte(encoder.nextNibble());
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void sendResponse(std::vector<uint8_t> &data)
{
	{
		std::lock_guard<std::mutex> lock(mtx);
		responsePending = true;
	}

	encoder.interruptWithControlBlock(data);
	while (encoder.hasData())
	{
		serial.writeByte(encoder.nextNibble());
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	{
		std::lock_guard<std::mutex> lock(mtx);
		responsePending = false;
	}
	cv.notify_all();
}

void connect()
{
	auto start = std::chrono::steady_clock::now();
	Logger(INFO) << "Connecting...";
	std::vector<uint8_t> data = cp.createControlBlock(Flags::CONNECT, 0);
	crc.attachCRC(data);
	while (!cp.isConnected())
	{
		std::thread sendThread(sendData, std::ref(data));
		sendThread.join();

		auto now = std::chrono::steady_clock::now();
		auto elapsed = duration_cast<std::chrono::seconds>(now - start).count();
		Logger(INFO, true) << "Connecting... " << elapsed << "s elapsed";
		std::this_thread::sleep_for(std::chrono::milliseconds(485));
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
			std::vector<uint8_t> response;
			if (valid)
			{
				outputData.insert(outputData.end(), data.begin(), data.end());
				response = cp.createControlBlock(Flags::RECEIVED, id);
			}
			else
			{
				response = cp.createControlBlock(Flags::RESEND, id);
			}
			outgoingResponseQueue.push(response);
		}
		else
		{
			throw std::invalid_argument("Invalid block type.");
		}
	}
}

void processOutgoingQueue()
{
	while (true)
	{
		if (!cp.isConnected())
		{
			if (cp.isCloseCmdReceived())
				return;

			connect();
		}

		if (!outgoingResponseQueue.empty())
		{
			std::vector<uint8_t> response;
			outgoingResponseQueue.wait_and_pop(response);
			sendResponseThread.join();
			sendResponseThread = std::thread(sendResponse, std::ref(response));
		}

		if (!sendDataThread.joinable())
			continue;

		if (nextPacketId < outgoingData.size())
		{
			std::vector<uint8_t> packet = outgoingData[nextPacketId++];
			sendDataThread.join();
			sendDataThread = std::thread(sendData, std::ref(packet));

			std::unique_lock<std::mutex> lock(mtx);
			if (cv.wait_for(lock, std::chrono::seconds(5), []
			{ return !cp.responses.empty(); }))
			{
				// Received a response within the timeout
				std::pair<uint16_t, Flags> response;
				cp.responses.wait_and_pop(response);

				if (response.second == Flags::RECEIVED)
				{
					// Continue with the next packet
					continue;
				}
				else if (response.second == Flags::RESEND)
				{
					errors++;
					nextPacketId = response.first;
				}
			}
			else
			{
				// Timeout occurred, resend the last packet
				nextPacketId--;
			}
		}
		else
		{
			std::vector<uint8_t> data = cp.createControlBlock(Flags::TRANSFER_FINISHED, 0);
			crc.attachCRC(data);
			sendDataThread.join();
			sendDataThread = std::thread(sendData, std::ref(data));
		}
	}
}

void watchControlPanel()
{
	while(!cp.isConnected()) {}

	Logger(DEBUG) << "Watching control panel...";
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
		{
			Logger(DEBUG) << "Connection closed!";
			return;
		}

		Logger(INFO, true) << "Sending packet " << nextPacketId << " of " << outgoingData.size() << " packets. Errors: " << errors;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void prepareOutgoingQueue(std::vector<uint8_t> &inputData)
{
	Logger(DEBUG) << "Preparing outgoing queue with " << inputData.size() << " bytes...";

	Logger(INFO) << "";
	while (!inputData.empty())
	{
		std::vector<uint8_t> data = {static_cast<uint8_t>(nextPacketId >> 8), static_cast<uint8_t>(nextPacketId++)};

		size_t bytesToSend = std::min(static_cast<size_t>(64), inputData.size());
		data.insert(data.end(), inputData.begin(), inputData.begin() + bytesToSend);

		uint32_t crcValue = crc.calculateCRC(data);
		data.push_back(crcValue >> 8);
		data.push_back(crcValue);

		outgoingData.push_back(data);
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
	std::thread outgoingThread(processOutgoingQueue);

	std::thread watchThread(watchControlPanel);

	receiveThread.join();
	incomingThread.join();
	outgoingThread.join();

	Logger(DEBUG) << "Queues stopped.";

	watchThread.join();

	serial.closePort();
	setBinaryOutput(outputData);
	return 0;
}
