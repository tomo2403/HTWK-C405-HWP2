#pragma once

#include <vector>
#include <cstdint>
#include "BlockType.hpp"
#include "ICommunicationInterface.hpp"
#include "ControlPanel.hpp"
#include "ThreadSafeQueue.hpp"

class DecoderObserver
{
private:
	ControlPanel* cp;
	ICommunicationInterface* serial;

public:
	ThreadSafeQueue<std::pair<BlockType, std::vector<uint8_t>>> incomingQueue; /**< A queue to store incoming packets. */

	DecoderObserver(ICommunicationInterface* serial, ControlPanel* cp);

	void beginBlockReceived(const BlockType &blockType);

	void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector);

	void receiveData();
};