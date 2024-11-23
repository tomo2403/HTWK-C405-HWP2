#pragma once

#include "IoManager.hpp"

class IoManagerB15F : IoManager
{
private:
	void getContinuesInput();

	void sendPacket(const StreamPacket &sp);

	void receivePacket(StreamPacket &sp);

public:
	IoManagerB15F(CRC crc, uint8_t outboundChannel);
};
