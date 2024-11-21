#include "../header/IoManagerPc.hpp"

IoManagerPc::IoManagerPc(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel) : IoManager(escapeSequence, crc, outboundChannel)
{

}

void IoManagerPc::sendPacket(const StreamPacket &sp)
{
	std::vector<uint8_t> packetData = {sp.packet.index};
	packetData.insert(packetData.end(), sp.packet.data.begin(), sp.packet.data.end());
	packetData.push_back(sp.packet.crc);
	Encoder enc(escapeSequence, packetData);

	while (enc.hasData())
	{
		uint8_t message = enc.nextByte().value();
		ssize_t n = write(serialPort, &message, sizeof(message));
		if (n < 0)
		{
			throw std::runtime_error("Error writing to serial port!");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
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
	p.data.push_back(packetIndex);
	p.data.push_back(success ? 0x01 : 0x00);

	StreamPacket sp{};
	sp.channel = channel;
	sp.type = PacketType::Response;
	sp.packet = p;
}