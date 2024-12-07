#include "ComInterface.hpp"

void ComInterface::openCom()
{
	drv.setRegister(&DDRA, 0x0F);
}

void ComInterface::closeCom()
{

}

void ComInterface::writeByte(uint8_t data)
{
	std::lock_guard<std::mutex> lock(drvMutex);
	drv.setRegister(&PORTA, data);
}

void ComInterface::readByte(uint8_t &data)
{
	std::lock_guard<std::mutex> lock(drvMutex);
	data = drv.getRegister(&PINA) >> 4;
}

bool ComInterface::isDataAvailable()
{
	uint8_t nibble;
	readByte(nibble);
	if (nibble != previouslyReceivedNibble)
	{
		previouslyReceivedNibble = nibble;
		return true;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	return false;
}