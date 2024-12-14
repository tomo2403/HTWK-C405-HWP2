#include <utility>

#include "../header/ComManager.hpp"

ComManager::ComManager(ICommunicationInterface *com) : com(com)
{
}

void ComManager::sendData(const std::vector<uint8_t> &data)
{
	encoder.inputDataBlock(data);
	while (encoder.hasData())
	{
		std::unique_lock lock(mtx);
		while (responsePending)
		{
			cv.wait(lock);
		}

		com->writeByte(encoder.nextNibble());
	}
}

void ComManager::sendResponse(const std::vector<uint8_t> &data)
{
	{
		std::lock_guard lock(mtx);
		responsePending = true;
	}

	encoder.interruptWithControlBlock(data);
	while (encoder.hasData())
	{
		com->writeByte(encoder.nextNibble());
	}

	{
		std::lock_guard lock(mtx);
		responsePending = false;
	}
	cv.notify_all();
}

void ComManager::prepareOutgoingQueue(std::vector<uint8_t> inputData)
{
	Logger(DEBUG) << "Preparing outgoing queue with " << inputData.size() << " bytes...";

	Logger(INFO) << "";
	uint16_t i = 0;
	while (!inputData.empty())
	{
		std::vector data = {static_cast<uint8_t>(i >> 8), static_cast<uint8_t>(i++)};

		const size_t bytesToSend = std::min(static_cast<size_t>(64), inputData.size());
		data.insert(data.end(), inputData.begin(), inputData.begin() + bytesToSend);

		const uint32_t crcValue = crc.calculateCRC(data);
		data.push_back(crcValue >> 24);
		data.push_back((crcValue >> 16) & 0xFF);
		data.push_back((crcValue >> 8) & 0xFF);
		data.push_back(crcValue & 0xFF);

		outgoingData.push_back(data);
		inputData.erase(inputData.begin(), inputData.begin() + bytesToSend);
		Logger(INFO, true) << "Data left to process: " << inputData.size() << " bytes";
	}
}

void ComManager::connect()
{
	const auto start = std::chrono::steady_clock::now();
	Logger(INFO) << "Connecting...";
	std::vector<uint8_t> data = cp.createControlBlock(CONNECT, 0);
	crc.attachCRC(data);
	while (!cp.isConnected())
	{
		std::thread sendThread(&ComManager::sendData, this, std::ref(data));
		sendThread.join();

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		Logger(INFO, true) << "Connecting... " << elapsed << "s elapsed";
		std::this_thread::sleep_for(std::chrono::milliseconds(480));
	}
	Logger(INFO) << "Connected!";
}

void ComManager::receiveData()
{
	decoder.addObserver(this);

	while (cp.isConnected() || !cp.isCloseCmdReceived())
	{
		std::lock_guard lock(mtx);
		if (!com->isDataAvailable())
			continue;

		uint8_t byte;
		com->readByte(byte);
		decoder.nextNibble(byte);
	}
}

void ComManager::processIncomingQueue()
{
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
			return;

		if (incomingQueue.empty())
			continue;

		std::pair<BlockType, std::vector<uint8_t> > packet;
		incomingQueue.wait_and_pop(packet);

		const uint32_t id = (packet.second[0] << 8) | packet.second[1];
		std::vector data(packet.second.begin() + 2, packet.second.end() - 4);
		const uint32_t receivedCrc = (packet.second[packet.second.size() - 4] << 24) | (packet.second[packet.second.size() - 3] << 16) |
		                             (packet.second[packet.second.size() - 2] << 8) | packet.second[packet.second.size() - 1];
		const bool valid = crc.validateCRC({packet.second.begin(), packet.second.end() - 4}, receivedCrc);

		if (packet.first == controlBlock)
		{
			if (valid)
				cp.processControlBlock(packet.second[2], id);
		}
		else if (packet.first == dataBlock)
		{
			std::vector<uint8_t> response;
			if (valid)
			{
				outputData.insert(outputData.end(), data.begin(), data.end());
				response = cp.createControlBlock(RECEIVED, id);
			}
			else
			{
				response = cp.createControlBlock(RESEND, id);
			}
			outgoingControlQueue.push(response);
		}
		else
		{
			throw std::invalid_argument("Invalid block type.");
		}
	}
}

void ComManager::processOutgoingQueue()
{
	std::promise<void> sendDataPromise;
	const std::future<void> sendDataFuture = sendDataPromise.get_future();

	std::promise<void> sendResponsePromise;
	std::future<void> sendResponseFuture = sendResponsePromise.get_future();

	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
			return;

		if (!cp.isConnected())
			continue;

		if (!outgoingControlQueue.empty())
		{
			if (sendResponseThread.joinable())
			{
				sendResponseThread.join();
			}
			std::vector<uint8_t> response;
			outgoingControlQueue.wait_and_pop(response);
			sendResponseThread = std::thread(&ComManager::sendResponse, this, std::ref(response));
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
			sendDataThread = std::thread(&ComManager::sendData, this, std::ref(packet));

			std::unique_lock lock(mtx);
			if (cv.wait_for(lock, std::chrono::seconds(10), [this] { return !cp.responses.empty(); }))
			{
				// Received a response within the timeout
				std::pair<uint16_t, Flags> response;
				cp.responses.wait_and_pop(response);

				if (response.second == RECEIVED)
				{
					// Continue with the next packet
					continue;
				}

				if (response.second == RESEND)
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
			std::vector<uint8_t> data = cp.createControlBlock(TRANSFER_FINISHED, 0);
			crc.attachCRC(data);
			if (sendResponseThread.joinable())
			{
				sendResponseThread.join();
			}
			sendResponseThread = std::thread(&ComManager::sendResponse, this, std::ref(data));
		}
	}
}

void ComManager::watchControlPanel() const
{
	while (!cp.isConnected())
	{
	}

	const auto start = std::chrono::steady_clock::now();
	Logger(DEBUG) << "Watching control panel...";
	while (true)
	{
		if (!cp.isConnected() && cp.isCloseCmdReceived())
		{
			Logger(DEBUG) << "Connection closed!";
			return;
		}

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		Logger(INFO, true) << "Packet " << nextPacketId << "/" << outgoingData.size() << " | Errors: " << errors << " | " << elapsed <<
				"s elapsed";
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

std::vector<uint8_t> ComManager::transfer2Way(std::vector<uint8_t> inputData)
{
	prepareOutgoingQueue(std::move(inputData));

	connectThread = std::thread(&ComManager::connect, this);
	std::thread watchThread(&ComManager::watchControlPanel, this);

	Logger(DEBUG) << "Starting queue threads...";
	std::thread receiveThread(&ComManager::receiveData, this);
	std::thread incomingThread(&ComManager::processIncomingQueue, this);

	connectThread.join();
	std::thread outgoingThread(&ComManager::processOutgoingQueue, this);

	receiveThread.join();
	incomingThread.join();
	outgoingThread.join();
	watchThread.join();

	Logger(DEBUG) << "Queues stopped.";
	return outputData;
}

void ComManager::beginBlockReceived(const BlockType &blockType)
{
	if (!cp.isConnected() && cp.isCloseCmdReceived())
	{
		cp.connect();
	}
}

void ComManager::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
	if (dataVector.size() < 7)
		return;
	incomingQueue.push(std::make_pair(blockType, dataVector));
}
