#pragma once

#include "IoManager.hpp"

class IoManagerPc : public IoManager
{
private:
	void sendPacket(const StreamPacket &sp) override;

	void receivePacket(StreamPacket &sp) override;

	bool checkResponse() override;

	void sendResponse(uint8_t channel, u_long packetIndex, bool success) override;

public:
	IoManagerPc(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel);
};
