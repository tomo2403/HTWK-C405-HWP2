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

	void getContinuesInput() override;

	void sendPacket(const StreamPacket &sp) override;

	void receivePacket(StreamPacket &sp) override;

	bool pinHasChanged(const uint8_t &channel);

public:
	IoManagerB15F(CRC crc, uint8_t outboundChannel, uint8_t inboundChannel);

	void transfer2Way(std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData);
};
