#include <algorithm>

#include "../header/ControlPacketDisassembler.hpp"
#include "../header/ControlPacketAssembler.hpp"
#include "../utilities.hpp"

#include <stdexcept>

void ControlPacketDisassembler::addObserver(IControlPacketDisassemblerObserver *observer)
{
	observers.push_back(observer);
}

void ControlPacketDisassembler::removeObserver(IControlPacketDisassemblerObserver *observer)
{
	std::erase(observers, observer);
}

uint32_t ControlPacketDisassembler::packetDisassembly_getId(const std::vector<uint8_t> &packet)
{
	uint32_t id = 0x00;
	id = (id | packet.at(0)) << 8;
	id = (id | packet.at(1)) << 8;
	id = (id | packet.at(2)) << 0;

	return id;
}

uint32_t ControlPacketDisassembler::packetDisassembly_getCrc(const std::vector<uint8_t> &packet)
{
	uint32_t crc = 0x00;
	crc = (crc | packet.at(packet.size() - 4)) << 8;
	crc = (crc | packet.at(packet.size() - 3)) << 8;
	crc = (crc | packet.at(packet.size() - 2)) << 8;
	crc = (crc | packet.at(packet.size() - 1)) << 0;

	return crc;
}

void ControlPacketDisassembler::packetDisassembly_processFlags(const std::vector<uint8_t> &packet, const uint32_t &id) const
{
	const auto flag = static_cast<Flag>(packet.at(3));
	for (const auto observer: observers)
	{
		switch (flag)
		{
			case TRANSFER_FINISHED:
				observer->on_transferFinished_received();
				break;
			case CLOSE_CONNECTION:
				observer->on_closeConnection_received();
				break;
			case RESEND:
				observer->on_resend_received(id);
				break;
			case CONNECT:
				observer->on_connect_received();
				break;
			case RECEIVED:
				observer->on_received_received(id);
				break;
			default:
				break;
		}
	}
}

void ControlPacketDisassembler::notifyOnCorrupt() const
{
	for (const auto observer: observers)
	{
		observer->on_packetCorrupt();
	}
}

void ControlPacketDisassembler::processPacket(const std::vector<uint8_t> &packet) const
{
	if (packet.size() != 8)
	{
		//throw std::invalid_argument("ControlPacketDisassembler: Packets must be exactly of size 8.");
		notifyOnCorrupt();
		return;
	}

	const uint32_t id = packetDisassembly_getId(packet);
	const uint32_t crc = packetDisassembly_getCrc(packet);

	if (!CRC::validateCRC(utilities::extractSubvector(packet, 0, 4), crc))
	{
		notifyOnCorrupt();
	}
	else
	{
		packetDisassembly_processFlags(packet, id);
	}
}
