#pragma once

#include "cstdint"
#include "vector"
#include "thread"
#include <future>

#include "Logger.hpp"
#include "ControlPanel.hpp"
#include "Encoder.hpp"
#include "Decoder.hpp"
#include "CRC.hpp"

class ComManager
{
private:
	std::mutex mtx;
	std::condition_variable cv;
	bool responsePending = false;

	std::thread sendDataThread;
	std::thread sendResponseThread;

	ControlPanel cp;
	Encoder encoder;
	Decoder decoder;
	CRC crc;
	ICommunicationInterface* com;
	DecoderObserver observer = DecoderObserver(&cp);

	std::vector<uint8_t> outputData{};

	std::vector<std::vector<uint8_t>> outgoingData{};
	ThreadSafeQueue<std::vector<uint8_t>> outgoingResponseQueue;
	uint16_t nextPacketId = 0;
	u_long errors = 0;

	void sendData(const std::vector<uint8_t> &data);

	void sendResponse(const std::vector<uint8_t> &data);

public:

	explicit ComManager(ICommunicationInterface* com);

	void prepareOutgoingQueue(std::vector<uint8_t> inputData);

	void connect();

	void receiveData();

	void processIncomingQueue();

	void processOutgoingQueue();

	void watchControlPanel();

	std::vector<uint8_t> transfer2Way();
};
