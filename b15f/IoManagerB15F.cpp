#include "IoManagerB15F.hpp"

IoManagerB15F::IoManagerB15F(CRC crc, uint8_t outboundChannel, uint8_t inboundChannel) : IoManager(crc, outboundChannel, inboundChannel)
{
	this->prevNibbleOnChannelZero = b15.getPINA() & 0x0F;
	this->prevNibbleOnChannelFour = b15.getPINA() & 0xF0;
}

void IoManagerB15F::sendPacket(const StreamPacket &sp)
{
	Encoder encoder = Encoder(sp.data);

	std::cerr << "Test ich hasse mein Leben" << std::endl;

	switch (sp.channel)
	{
		case 0:
			// bit 7-4 unchanged, bit 3-0 output
			b15.setDDRA((b15.getDDRA() | 0x0F));
			break;
		case 4:
			// bit 7-4 output, bit 3-0 unchanged
			b15.setDDRA((b15.getDDRA() | 0xF0));
			break;
	}

	while (encoder.hasData())
	{
		b15.setPORTA(encoder.nextNibble() << sp.channel);
	}
}

bool IoManagerB15F::pinHasChanged(const uint8_t &channel)
{
	uint8_t prevNibble = channel == 0 ? prevNibbleOnChannelZero : prevNibbleOnChannelFour;

	uint8_t currentNibble = b15.getPINA() & (0x0F << channel);
	return prevNibble != currentNibble;
}

void IoManagerB15F::receivePacket(StreamPacket &sp)
{
	std::vector<uint8_t> encodedData = std::vector<uint8_t>();
	Decoder decoder(encodedData);

	const uint8_t channelToReadFrom = awaitingResponse ? inboundChannel : outboundChannel;

	while (!decoder.hasData())
	{
		if (pinHasChanged(channelToReadFrom))
		{
			decoder.nextNibble(b15.getPINA() & (0x0F << channelToReadFrom));
		}
	}

	sp.channel = channelToReadFrom;
	sp.dataLength = encodedData.size();
	sp.data = encodedData;
}

void IoManagerB15F::getContinuesInput()
{
	while (true)
	{
		StreamPacket sp{};
		receivePacket(sp);
		incomingQueue.push(sp);
	}
}