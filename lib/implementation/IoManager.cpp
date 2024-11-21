#include "../header/IoManager.hpp"

IoManager::IoManager(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel) : escapeSequence(escapeSequence), crc(crc), outboundChannel(outboundChannel)
{

}

void IoManager::send()
{
	while (!connected)
	{ }

	u_long index = 0;

	for (int i = 0; i < packets.size(); i++) {
		Encoder enc = Encoder(escapeSequence, Encoder::convertPacket(packets[i]));

		StreamPacket sp{};
		sp.channel = outboundChannel;
		sp.dataLength = packets[i].data.size();


		sp.data = enc.encodeAll();

		sendPacket(sp);

		if (!checkResponse()) {
			// resend packet
			i--;
		}
	}

	// TODO: send close connection command
}

std::vector<uint8_t> IoManager::getBinaryPipeContent()
{
	std::vector<uint8_t> data;
	char ch;
	while (std::cin.get(ch))
	{
		data.push_back(static_cast<uint8_t>(ch));
	}
	if (std::cin.eof())
	{
		std::cin.clear();
	}
	return data;
}

void IoManager::printVector(const std::vector<uint8_t> &data)
{
	std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
}

void IoManager::preparePackets(std::vector<uint8_t> &data)
{
	u_long dataIndex = 0;
	u_long packetIndex = 0;

	while (dataIndex < data.size())
	{
		PrePacket p{};
		p.index = packetIndex++;
		for (int i = 0; i < 64 && dataIndex < data.size(); i++)
		{
			p.data.push_back(data[dataIndex++]);
		}
		crc.calculateCRC(p);
		packets.push_back(p);
	}
}

bool IoManager::checkResponse()
{
	// TODO: check if response is valid
	return true;
}

void IoManager::sendResponse(uint8_t channel, u_long packetIndex, bool success)
{
	PrePacket p{};
	p.index = packetIndex;
	p.data.push_back(success ? 0x01 : 0x00);
	crc.calculateCRC(p);

	Encoder enc = Encoder(escapeSequence, Encoder::convertPacket(p));

	StreamPacket sp{};
	sp.channel = channel;
	sp.data = enc.encodeAll();

	sendPacket(sp);
}

void IoManager::transfer2Way(std::vector<uint8_t> &input, std::vector<uint8_t> &output)
{
	preparePackets(input);

	// TODO: implement
}
