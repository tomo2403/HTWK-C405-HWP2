#include "../lib/lib.hpp"
#include <condition_variable>
#include <mutex>
#include <future>
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

void sendData(const std::vector<uint8_t> &data)
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
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
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
	Logger(DEBUG) << "Watching control panel...";
	std::promise<void> sendDataPromise;
	std::future<void> sendDataFuture = sendDataPromise.get_future();

	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
			return;

		if (!cp.isConnected())
			continue;

		if (!outgoingResponseQueue.empty())
		{
			if (sendResponseThread.joinable())
			{
				sendResponseThread.join();
				std::vector<uint8_t> response;
				outgoingResponseQueue.wait_and_pop(response);
				sendResponseThread = std::thread(sendResponse, std::ref(response));
			}
		}

		if (sendDataFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			continue;

		if (nextPacketId < outgoingData.size())
		{
			std::vector<uint8_t> packet = outgoingData[nextPacketId++];
			if (sendDataThread.joinable())
			{
				sendDataThread.join();
			}
			sendDataPromise = std::promise<void>();
			sendDataFuture = sendDataPromise.get_future();
			sendDataThread = std::thread([&sendDataPromise](std::vector<uint8_t> &data)
										 {
											 sendData(data);
											 sendDataPromise.set_value();
										 }, std::ref(packet));

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
			if (sendDataThread.joinable())
			{
				sendDataThread.join();
			}
			sendDataPromise = std::promise<void>();
			sendDataFuture = sendDataPromise.get_future();
			sendDataThread = std::thread([&sendDataPromise](std::vector<uint8_t> &data)
										 {
											 sendData(data);
											 sendDataPromise.set_value();
										 }, std::ref(data));
		}

		Logger(INFO, true) << "Sending packet " << nextPacketId << " of " << outgoingData.size() << " packets. Errors: " << errors;
	}
}

void watchControlPanel()
{
	while (!cp.isConnected())
	{
		connect();
	}

	Logger(DEBUG) << "Watching control panel...";
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
		{
			Logger(DEBUG) << "Connection closed!";
			return;
		}

		Logger(INFO, true) << "Sending packet " << nextPacketId << " of " << outgoingData.size() << " packets. Errors: " << errors;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void prepareOutgoingQueue(std::vector<uint8_t> &inputData)
{
	Logger(DEBUG) << "Preparing outgoing queue with " << inputData.size() << " bytes...";

	Logger(INFO) << "";
	uint16_t i = 0;
	while (!inputData.empty())
	{
		std::vector<uint8_t> data = {static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i++)};

		size_t bytesToSend = std::min(static_cast<size_t>(64), inputData.size());
		data.insert(data.end(), inputData.begin(), inputData.begin() + bytesToSend);

		uint32_t crcValue = crc.calculateCRC(data);
		data.push_back(crcValue >> 24);
		data.push_back((crcValue >> 16) & 0xFF);
		data.push_back((crcValue >> 8) & 0xFF);
		data.push_back(crcValue & 0xFF);

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