#pragma once

#include <b15f/b15f.h>

#include "../lib/header/IoManager.hpp"

#include "B15Communication.hpp"

class IoManagerB15F : public IoManager
{
private:
	uint8_t prevNibbleOnChannelZero;
	uint8_t prevNibbleOnChannelFour;

	B15Communication b15;

	[[noreturn]] void getContinuesInput() override;

	void sendPacket(const StreamPacket &sp) override;

	void receivePacket(StreamPacket &sp) override;

	bool pinHasChanged(const uint8_t &channel);

public:
	IoManagerB15F(CRC crc, uint8_t outboundChannel, uint8_t inboundChannel);
};
