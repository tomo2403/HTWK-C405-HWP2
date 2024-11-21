#include <b15f/b15f.h>
#include "../lib/header/IoManagerB15F.hpp"

typedef IoManagerB15F ThisIoManager;

int main()
{
    CRC crc(65, 0x04C11DB7);
    ThisIoManager ioManager = ThisIoManager(0x80, crc, 0x02);
    ioManager.openSerialPort();

    std::vector<uint8_t> inputData = ThisIoManager::getBinaryPipeContent();
    std::vector<uint8_t> outputData;

    ioManager.preparePackets(inputData);

    std::thread writer(&ThisIoManager::send, &ioManager);
    std::thread reader(&ThisIoManager::receive, &ioManager, std::ref(outputData));

    writer.join();
    reader.join();

    ioManager.closeSerialPort();
    ThisIoManager::printVector(outputData);

    return 0;
}