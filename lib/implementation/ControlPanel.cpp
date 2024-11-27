#include "../header/ControlPanel.hpp"

bool ControlPanel::isConnected() const
{
	return connected;
}

bool ControlPanel::isEverythingSent() const
{
	return everythingSent;
}

bool ControlPanel::isEverythingReceived() const
{
	return everythingReceived;
}

bool ControlPanel::isCloseCmdReceived() const
{
	return closeCmdReceived;
}

void ControlPanel::processControlBlock(uint8_t &flags, uint8_t &packetId)
{
    if (flags & Flags::CLOSE_CONNECTION)
        closeCmdReceived = true;
    else if (flags & Flags::TRANSFER_FINISHED)
        everythingReceived = true;
    else if (flags & Flags::RESEND)
        resendRequests.push(packetId);
    else if (flags & Flags::CONNECT)
        connected = true;
    else if (flags & Flags::RECEIVED)
        confirmedPackets.push(packetId);
}

std::array<uint8_t, 2> ControlPanel::createControlBlock(uint8_t flags, uint8_t packetId)
{
    std::array<uint8_t, 2> controlBlock{flags, packetId};

    if (flags & Flags::TRANSFER_FINISHED)
        everythingSent = true;
    else if (flags & Flags::CLOSE_CONNECTION)
        connected = false;

    return controlBlock;
}
