#pragma once

#include <atomic>
#include <cstdint>

#include "Timer.hpp"
#include "Logger.hpp"

class ControlPanel
{
	std::atomic<uint32_t> sendPackets = 0;
	std::atomic<uint32_t> receivedPackets = 0;
	std::atomic<uint32_t> lostPackets = 0;
	std::atomic<uint32_t> resentPackets = 0;
	std::atomic<uint32_t> receivedFaultyPackets = 0;
	std::atomic<bool> connected = false;
	std::atomic<bool> finishedSending = false;
	std::atomic<bool> finishedReceiving = false;

	Timer connectionTime;
	Timer sendingTime;
	Timer transferTime;
	Timer disconnectionTime;

	void checkTransferStatus();

public:
	ControlPanel();
	~ControlPanel();

	void updateLog();

	void incrementSendPackets();
	void incrementReceivedPackets();
	void incrementLostPackets();
	void incrementResentPackets();
	void incrementReceivedFaultyPackets();
	void setConnected(bool connected);
	void finishSending();
	void finishReceiving();
};
