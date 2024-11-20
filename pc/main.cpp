#include <cstdint>
#include <vector>
#include <thread>
#include <bitset>

#include "../lib/IoManager.hpp"

int main()
{
	CRC crc(65, 0x04C11DB7);
	IoManager ioManager = IoManager(0x80, crc);
	ioManager.openSerialPort();

	std::vector<uint8_t> inputData = IoManager::getBinaryPipeContent();
	std::vector<uint8_t> outputData;

	ioManager.preparePackets(inputData);

	std::thread writer(&IoManager::send, &ioManager, std::ref(inputData));
	std::thread reader(&IoManager::receive, &ioManager, std::ref(outputData));

	writer.join();
	reader.join();

	ioManager.closeSerialPort();
	IoManager::printVector(outputData);

	return 0;
}