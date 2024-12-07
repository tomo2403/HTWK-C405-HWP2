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

public:
	ThreadSafeQueue<std::pair<BlockType, std::vector<uint8_t>>> incomingQueue; /**< A queue to store incoming packets. */

	explicit DecoderObserver(ControlPanel* cp);

	void beginBlockReceived(const BlockType &blockType);

	void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector);
};