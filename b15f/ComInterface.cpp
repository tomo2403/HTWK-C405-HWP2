#include "ComInterface.hpp"

void ComInterface::openCom()
{
	drv.setRegister(&DDRA, 0x0F);
	Logger(INFO) << "B15F is connected.";
}

void ComInterface::closeCom()
{

}

void ComInterface::writeByte(uint8_t data)
{
	std::lock_guard lock(drvMutex);
	drv.setRegister(&PORTA, data);
}

std::optional<uint8_t> ComInterface::readByte()
{
	std::lock_guard lock(drvMutex);
	uint8_t byte = drv.getRegister(&PINA) >> 4;

	if (byte == previouslyReceivedNibble)
	{
		return std::nullopt;
	}

	previouslyReceivedNibble = byte;
	return byte;
}
