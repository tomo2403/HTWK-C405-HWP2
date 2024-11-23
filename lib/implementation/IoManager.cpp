#include "../header/IoManager.hpp"

IoManager::IoManager(CRC crc, uint8_t outboundChannel) : crc(crc), outboundChannel(outboundChannel)
{

}

std::vector<uint8_t> IoManager::getBinaryInput()
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

void IoManager::setBinaryOutput(const std::vector<uint8_t> &data)
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
		outgoingPackets.push_back(p);
	}
}

void IoManager::sendResponse(uint8_t channel, u_long packetIndex, bool success)
{
	sendData(channel, packetIndex, std::vector<uint8_t> { success });
}

void IoManager::sendData(uint8_t channel, u_long packetIndex, std::vector<uint8_t> data)
{
	PrePacket p{};
	p.index = packetIndex;
	p.data = data;
	crc.calculateCRC(p);

	Encoder enc = Encoder(Encoder::convertPacket(p));

	StreamPacket sp{};
	sp.channel = channel;
	sp.data = enc.encodeAll();

	sendPacket(sp);
}

void IoManager::processIncomingPacket(StreamPacket &sp)
{
	Decoder dec = Decoder(sp.data);
	PrePacket p = dec.decodeAll(sp.data);

	bool packetValid = crc.validateCRC(p);

	if (!(awaitingResponse && p.data[0] == true && packetValid))
		sendResponse(sp.channel, p.index, packetValid);

	if (awaitingResponse) {
		awaitingResponse = !awaitingResponse;

		if (p.data[0] == false) {
			StreamPacket response = createStreamPacket(outgoingPackets[p.index], sp.channel);
			outgoingQueue.push(response);
		}
	}
	else if (packetValid) {
		receivedPackets.push_back(p);
	}
}

StreamPacket IoManager::createStreamPacket(PrePacket p, uint8_t channel)
{
	Encoder enc = Encoder(Encoder::convertPacket(p));
	StreamPacket sp{};
	sp.channel = channel;
	sp.data = enc.encodeAll();
	sp.dataLength = sp.data.size();
	return sp;
}

void IoManager::transfer2Way(std::vector<uint8_t> &input, std::vector<uint8_t> &output)
{
	preparePackets(input);

	std::thread handleInput(&IoManager::getContinuesInput, this);

	while (!connected)
	{ }

	while (!incomingQueue.empty() && !outgoingQueue.empty())
	{
		// TODO: implement

		if (!incomingQueue.empty())
		{
			StreamPacket sp;
			incomingQueue.wait_and_pop(sp);
			processIncomingPacket(sp);
		}

		if (!outgoingQueue.empty())
		{
			StreamPacket sp;
			outgoingQueue.wait_and_pop(sp);
			sendPacket(sp);
		}
	}
}
