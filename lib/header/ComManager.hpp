#pragma once

#include <cstdint>
#include <vector>
#include <thread>

#include "Logger.hpp"
#include "ControlPanel.hpp"
#include "Encoder.hpp"
#include "Decoder.hpp"
#include "CRC.hpp"
#include "IDecoderObserver.hpp"
#include "ICommunicationInterface.hpp"

class ComManager final : IDecoderObserver
{
	mutable std::mutex mtx;

	std::thread sendDataThread;
	std::thread connectThread;

	std::vector<std::vector<uint8_t> > outgoingData{};
	ThreadSafeQueue<std::vector<uint8_t> > outgoingControlQueue;
	ThreadSafeQueue<std::pair<BlockType, std::vector<uint8_t> > > incomingQueue;
	uint8_t lastSendBlock = 0;
	u_long errors = 0;

	ControlPanel cp;
	Encoder encoder;
	Decoder decoder;
	CRC crc;
	ICommunicationInterface *com;

	std::vector<uint8_t> outputData{};


	void sendData();

public:
	explicit ComManager(ICommunicationInterface *com);

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
