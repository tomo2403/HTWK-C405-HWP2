#pragma once

#include <cstdint>
#include <vector>
#include <thread>
#include <future>

#include "Logger.hpp"
#include "ControlPanel.hpp"
#include "Encoder.hpp"
#include "Decoder.hpp"
#include "CRC.hpp"
#include "ICommunicationInterface.hpp"

class ComManager final : DecoderObserver
{
private:
	mutable std::mutex mtx;
	std::condition_variable cv;
	bool responsePending = false;

	std::thread sendDataThread;
	std::thread sendResponseThread;
	std::thread connectThread;

	std::vector<std::vector<uint8_t>> outgoingData{};
	ThreadSafeQueue<std::vector<uint8_t>> outgoingControlQueue;
	ThreadSafeQueue<std::pair<BlockType, std::vector<uint8_t>>> incomingQueue;
	uint16_t nextPacketId = 0;
	u_long errors = 0;

	ControlPanel cp;
	Encoder encoder;
	Decoder decoder;
	CRC crc;
	ICommunicationInterface* com;

	std::vector<uint8_t> outputData{};


	void sendData(const std::vector<uint8_t> &data);

	void sendResponse(const std::vector<uint8_t> &data);

public:

	explicit ComManager(ICommunicationInterface* com);

	void prepareOutgoingQueue(std::vector<uint8_t> inputData);

	void connect();

	void receiveData();

	void processIncomingQueue();

	void processOutgoingQueue();

	void watchControlPanel() const;

	std::vector<uint8_t> transfer2Way(std::vector<uint8_t> inputData);

	void beginBlockReceived(const BlockType &blockType) override;

	void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) override;
};
