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

void ControlPanel::processControlBlock(uint8_t &data)
{

}
