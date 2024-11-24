#include "IoManagerB15F.hpp"

int main()
{
    CRC crc(65, 0x04C11DB7);
    IoManagerB15F ioManager(crc, 0x0, 0x04);

    std::vector<uint8_t> inputData = IoManager::getBinaryInput();
    std::vector<uint8_t> outputData;

    std::thread transfer(&IoManagerB15F::transfer2Way, &ioManager, std::ref(inputData), std::ref(outputData));

    transfer.join();

    IoManager::setBinaryOutput(outputData);

    return 0;
}