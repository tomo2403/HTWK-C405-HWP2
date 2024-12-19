#include "../header/ControlPanel.hpp"

bool ControlPanel::isConnected() const
{
	std::lock_guard lock(mtx);
	return connected;
}

bool ControlPanel::isEverythingSent() const
{
	std::lock_guard lock(mtx);
	return everythingSent;
}

bool ControlPanel::isEverythingReceived() const
{
	std::lock_guard lock(mtx);
	return everythingReceived;
}

bool ControlPanel::isCloseCmdReceived() const
{
	std::lock_guard lock(mtx);
	return closeCmdReceived;
}

void ControlPanel::processControlBlock(const uint8_t &flags, uint32_t packetId)
{
	std::lock_guard lock(mtx);
	if (flags & CLOSE_CONNECTION)
		closeCmdReceived = true;
	if (flags & TRANSFER_FINISHED)
		everythingReceived = true;
	if (flags & AWAITING)
		packetRequests.push(packetId);
	if (flags & CONNECT)
		connected = true;
}

std::vector<uint8_t> ControlPanel::createControlBlock(const uint8_t flags, const uint32_t packetId, CRC* crc)
{
	std::vector<uint8_t> data;
	data.push_back(packetId >> 8);
	data.push_back(packetId);
	data.push_back(flags);

	crc->attachCRC(data);

	std::lock_guard lock(mtx);
	if (flags & TRANSFER_FINISHED)
		everythingSent = true;
	if (flags & CLOSE_CONNECTION)
		connected = false;

	return data;
}

void ControlPanel::connect()
{
	std::lock_guard lock(mtx);
	connected = true;
}
