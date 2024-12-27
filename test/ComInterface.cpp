#include "ComInterface.hpp"

void ComInterface::openCom()
{
}

void ComInterface::closeCom()
{
}

void ComInterface::writeByte(const uint8_t data)
{
	Logger(DEBUG) << "Writing byte: " << std::hex << static_cast<int>(data);
	queue.push(data);
}

void ComInterface::readByte(uint8_t &data)
{
	data = queue.wait_and_pop();
	Logger(DEBUG) << "Reading byte: " << std::hex << static_cast<int>(data);
}

bool ComInterface::isDataAvailable()
{
	return !queue.empty();
}
