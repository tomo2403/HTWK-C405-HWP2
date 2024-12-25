#include "ComInterface.hpp"

void ComInterface::openCom()
{
	std::lock_guard lock(mtx);
}

void ComInterface::closeCom()
{
	std::lock_guard lock(mtx);
}

void ComInterface::writeByte(const uint8_t data)
{
	std::lock_guard lock(mtx);
	Logger(DEBUG) << "Writing byte: " << std::hex << static_cast<int>(data);
}

void ComInterface::readByte(uint8_t &data)
{
	std::lock_guard lock(mtx);
    data = buffer[index++];
	Logger(DEBUG) << "Reading byte: " << std::hex << static_cast<int>(data);
}

bool ComInterface::isDataAvailable()
{
	return index < buffer.size();
}
