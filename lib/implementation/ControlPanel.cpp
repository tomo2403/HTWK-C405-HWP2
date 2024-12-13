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
    else if (flags & TRANSFER_FINISHED)
        everythingReceived = true;
    else if (flags & RESEND)
        responses.push(std::make_pair(packetId, RESEND));
    else if (flags & CONNECT)
        connected = true;
    else if (flags & RECEIVED)
		responses.push(std::make_pair(packetId, RECEIVED));
}

std::vector<uint8_t> ControlPanel::createControlBlock(const uint8_t flags, const uint32_t packetId)
{
	std::vector<uint8_t> data;
	data.push_back(packetId >> 8);
	data.push_back(packetId);
	data.push_back(flags);

	std::lock_guard lock(mtx);
    if (flags & TRANSFER_FINISHED)
        everythingSent = true;
    else if (flags & CLOSE_CONNECTION)
        connected = false;

    return data;
}

void ControlPanel::connect()
{
	std::lock_guard lock(mtx);
    connected = true;
}
