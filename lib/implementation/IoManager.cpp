#include "../header/IoManager.hpp"
#include <algorithm>
#include <utility>

IoManager::IoManager(CRC crc, uint8_t outboundChannel, uint8_t inboundChannel) : crc(crc), outboundChannel(outboundChannel),
																				 inboundChannel(inboundChannel)
{

}

StreamPacket IoManager::createStreamPacket(const PrePacket &p, uint8_t channel)
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
	std::cerr << "[DEBUG] Getting input data..." << std::endl;
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
	std::cerr << "[INFO ] Input data processed!" << std::endl;
	return data;
}

void IoManager::setBinaryOutput(const std::vector<uint8_t> &data)
{
	std::cerr << "[DEBUG] Writing output..." << std::endl;
	std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
	std::cerr << "[INFO ] Output written!" << std::endl;
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
		std::cerr << "\r[DEBUG] Processing " << dataIndex << "/" << data.size() << " bytes to " << packetIndex << " packets";
	}
	std::cerr << std::flush << std::endl;
}

void IoManager::sendResponse(uint8_t channel, u_long packetIndex, bool success)
{
	sendData(channel, packetIndex, std::vector<uint8_t>{success});
}

void IoManager::sendData(uint8_t channel, u_long packetIndex, std::vector<uint8_t> data)
{
	PrePacket p{};
	p.index = packetIndex;
	p.data = std::move(data);
	crc.calculateCRC(p);

	sendData(channel, p);
}

void IoManager::sendData(uint8_t channel, PrePacket p)
{
	//std::cerr << "[DEBUG] Sending data..." << std::endl;
	Encoder enc = Encoder(Encoder::convertPacket(p));

	StreamPacket sp{};
	sp.channel = channel;
	sp.data = enc.encodeAll();
	sp.dataLength = sp.data.size();

	sendPacket(sp);
}

void IoManager::processIncomingPacket(StreamPacket &sp)
{
	//std::cerr << "[DEBUG] Processing input packet..." << std::endl;

	Decoder dec(sp.data);
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
			receivedPackets.push_back(PrePacket{p.index, p.crc, p.data});
		}
	}

	if (outgoingQueue.empty() && outgoingPackets.empty() && !awaitingResponse && p.connectionClosed)
		connected = false;

	if (p.transferFinished)
		everythingReceived = true;
}

void IoManager::transfer2Way(std::vector<uint8_t> &input, std::vector<uint8_t> &output)
{
	std::cerr << "[INFO ] Preparing packets..." << std::endl;
	//preparePackets(input);

	std::cerr << "[INFO ] Listening on serial port..." << std::endl;
	std::thread handleInput(&IoManager::getContinuesInput, this);

	auto start = steady_clock::now();
	std::cerr << "[INFO ] Connecting..." << std::endl;
	while (!connected)
	{
		sendData(2, 0, {CodecCommand::iAmReady, 0, 0, 0, 0, 0});

		auto now = steady_clock::now();
		auto elapsed = duration_cast<seconds>(now - start).count();

		std::cerr << "\r[INFO ] " << elapsed << "s elapsed" << std::flush;
		std::this_thread::sleep_for(microseconds(500));
	}

	std::cerr << std::endl << "[INFO ] Connected! Starting transfer..." << std::endl;
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

		std::cerr << "\r[INFO ] Outgoing packets: " << outgoingPackets.size() << " / Incoming packets: " << receivedPackets.size()
				  << std::flush;
	}
	std::cerr << std::endl << "[WARN ] Connection closed!" << std::endl;

	std::cerr << "[DEBUG] Sorting received packets..." << std::endl;
	std::sort(receivedPackets.begin(), receivedPackets.end(), [](const PrePacket &a, const PrePacket &b)
	{ return a.index < b.index; });

	std::cerr << "[DEBUG] Flushing received packets..." << std::endl;
	for (const PrePacket &p: receivedPackets)
	{
		output.insert(output.end(), p.data.begin(), p.data.end());
	}
	std::cerr << "[INFO ] Transfer finished!" << std::endl;
}
