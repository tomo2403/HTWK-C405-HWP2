#include "../header/ControlPanel.hpp"

bool ControlPanel::isConnected() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return connected;
}

bool ControlPanel::isEverythingSent() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return everythingSent;
}

bool ControlPanel::isEverythingReceived() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return everythingReceived;
}

bool ControlPanel::isCloseCmdReceived() const
{
	std::lock_guard<std::mutex> lock(mtx);
	return closeCmdReceived;
}

void ControlPanel::processControlBlock(uint8_t &flags, uint32_t packetId)
{
	std::lock_guard<std::mutex> lock(mtx);
    if (flags & Flags::CLOSE_CONNECTION)
        closeCmdReceived = true;
    else if (flags & Flags::TRANSFER_FINISHED)
        everythingReceived = true;
    else if (flags & Flags::RESEND)
        responses.push(std::make_pair(packetId, Flags::RESEND));
    else if (flags & Flags::CONNECT)
        connected = true;
    else if (flags & Flags::RECEIVED)
		responses.push(std::make_pair(packetId, Flags::RECEIVED));
}

std::vector<uint8_t> ControlPanel::createControlBlock(uint8_t flags, uint32_t packetId)
{
	std::vector<uint8_t> data;
	data.push_back(packetId >> 8);
	data.push_back(packetId);
	data.push_back(flags);

	std::lock_guard<std::mutex> lock(mtx);
    if (flags & Flags::TRANSFER_FINISHED)
        everythingSent = true;
    else if (flags & Flags::CLOSE_CONNECTION)
        connected = false;

    return data;
}

void ControlPanel::connect()
{
	std::lock_guard<std::mutex> lock(mtx);
    connected = true;
}
