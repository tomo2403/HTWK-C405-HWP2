#pragma once

#include "IoManager.hpp"

class IoManagerB15F : IoManager
{
private:
	void sendPacket(const StreamPacket &sp) override;

	bool checkResponse() override;

	void sendResponse(uint8_t channel, u_long packetIndex, bool success) override;

public:
	IoManagerB15F(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel);
};
