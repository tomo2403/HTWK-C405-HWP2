#pragma once

#include "../lib/header/IDecoderObserver.hpp"
#include "../lib/header/Decoder.hpp"
#include "../lib/header/ControlPanel.hpp"
#include "Serial.hpp"

class SerialCommunication : IDecoderObserver
{
private:
	ControlPanel &cp;
	Serial &serial;
	Decoder decoder;

public:
	ThreadSafeQueue<std::pair<BlockType, std::vector<uint8_t>>> incomingQueue; /**< A queue to store incoming packets. */

	explicit SerialCommunication(Serial &serial, ControlPanel &cp);

	void beginBlockReceived(const BlockType &blockType) override;

	void endBlockReceived(const BlockType &blockType) override;

	void receiveData();
};
