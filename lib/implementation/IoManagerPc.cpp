#include "../header/IoManagerPc.hpp"

IoManagerPc::IoManagerPc(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel) : IoManager(escapeSequence, crc, outboundChannel)
{

}

void IoManagerPc::sendPacket(const StreamPacket &sp)
{
	serialWrite(sp.channel);
	serialWrite(sp.dataLength);
	for (uint8_t byte : sp.data)
	{
		serialWrite(byte);
	}
}

void IoManagerPc::receivePacket(StreamPacket &sp)
{
	uint8_t channel = serialRead();
	uint8_t dataLength = serialRead();
	std::vector<uint8_t> data;
	for (int i = 0; i < dataLength; i++)
	{
		data.push_back(serialRead());
	}
	sp.channel = channel;
	sp.dataLength = dataLength;
	sp.data = data;
}

bool IoManagerPc::checkResponse()
{
	std::vector<uint8_t> data;
	Decoder dec(escapeSequence, data);

	std::atomic<bool> timeoutFlag{false};
	std::thread timeoutThread(&IoManagerPc::checkTimeout, this, std::ref(timeoutFlag), 5000); // 5 seconds timeout

	uint8_t receivedData;
	ssize_t m = read(serialPort, &receivedData, sizeof(receivedData));
	if (m < 0)
	{
		throw std::runtime_error("Error reading from serial port!");
	}
	dec.nextNibble(receivedData);

	timeoutThread.join();

	if (timeoutFlag.load())
	{
		return false;
	}

	// TODO: check if response is valid
	return true;
}

void IoManagerPc::sendResponse(uint8_t channel, u_long packetIndex, bool success)
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