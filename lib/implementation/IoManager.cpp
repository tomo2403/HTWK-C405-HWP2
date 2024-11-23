#include "../header/IoManager.hpp"

IoManager::IoManager(CRC crc, uint8_t outboundChannel) : crc(crc), outboundChannel(outboundChannel)
{

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
	sendData(channel, packetIndex, std::vector<uint8_t>{success});
}

void IoManager::sendData(uint8_t channel, u_long packetIndex, std::vector<uint8_t> data)
{
	PrePacket p{};
	p.index = packetIndex;
	p.data = data;
	crc.calculateCRC(p);

	sendData(channel, p);
}

void IoManager::sendData(uint8_t channel, PrePacket p)
{
	Encoder enc = Encoder(Encoder::convertPacket(p));

	StreamPacket sp{};
	sp.channel = channel;
	sp.data = enc.encodeAll();
	sp.dataLength = sp.data.size();

	sendPacket(sp);
}

void IoManager::processIncomingPacket(StreamPacket &sp)
{
	Decoder dec = Decoder(sp.data);
	PostPacket p = dec.decodeAll(sp.data);

	bool packetValid = crc.validateCRC(p);

	// Keine Antwort senden wenn: Paket gültig, Antwort erwartet, kein Fehler bei vorheriger Übertragung
	if (!(packetValid && awaitingResponse && p.data[0] == true))
		sendResponse(sp.channel, p.index, packetValid);

	if (packetValid)
	{
		if (awaitingResponse)
		{
			awaitingResponse = false;

			if (p.data[0] == false)
			{
				StreamPacket response = createStreamPacket(outgoingPackets[p.index], sp.channel);
				outgoingQueue.push(response);
			}
		}
		else
		{
			receivedPackets.push_back(PrePacket { p.index, p.crc, p.data });
		}
	}

	if (outgoingQueue.empty() && outgoingPackets.empty() && !awaitingResponse && p.connectionClosed)
		connected = false;

	if (p.transferFinished)
		everythingReceived = true;
}

void IoManager::transfer2Way(std::vector<uint8_t> &input, std::vector<uint8_t> &output)
{
	preparePackets(input);
	std::thread handleInput(&IoManager::getContinuesInput, this);

	while (!connected)
	{ }

	while (connected)
	{
		if (!incomingQueue.empty())
		{
			StreamPacket sp;
			incomingQueue.wait_and_pop(sp);
			processIncomingPacket(sp);
		}

		if (outgoingQueue.empty() && !outgoingPackets.empty())
		{
			outgoingQueue.push(createStreamPacket(outgoingPackets.front(), outboundChannel));
			outgoingPackets.erase(outgoingPackets.begin());
		}

		if (!outgoingQueue.empty())
		{
			StreamPacket sp;
			outgoingQueue.wait_and_pop(sp);
			sendPacket(sp);
		}
	}

	// sortiere die empfangenen Pakete nach Index
	std::sort(receivedPackets.begin(), receivedPackets.end(), [](const PrePacket &a, const PrePacket &b)
	{ return a.index < b.index; });

	for (const PrePacket &p: receivedPackets)
	{
		output.insert(output.end(), p.data.begin(), p.data.end());
	}
}
