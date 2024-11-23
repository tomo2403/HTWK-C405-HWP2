#pragma once

#include <b15f/b15f.h>

#include "IoManager.hpp"
#include "Decoder.hpp"
#include "Encoder.hpp"

class IoManagerB15F : IoManager
{
private:
	uint8_t prevNibbleOnChannelZero;
	uint8_t prevNibbleOnChannelFour;

	B15F& drv = B15F::getInstance();

	void getContinuesInput();

	void sendPacket(const StreamPacket &sp);

	void receivePacket(StreamPacket &sp);

	bool pinHasChanged(const uint8_t &channel);

public:
	IoManagerB15F(CRC crc, uint8_t outboundChannel, uint8_t inboundChannel);
};
