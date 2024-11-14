#include "ioManager.h"

void ioManager::send(uint8_t **data)
{

}

void ioManager::receive(uint8_t **data)
{

}

std::vector<uint8_t> ioManager::getBinaryPipeContent()
{
	std::vector<uint8_t> data;
	char ch;
	while (std::cin.get(ch)) {
		data.push_back(static_cast<uint8_t>(ch));
	}
	return data;
}

void ioManager::printVector(const std::vector<uint8_t> &data)
{
	std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
}
