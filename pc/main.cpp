#include "../lib/header/IoManagerPc.hpp"

int main()
{
    CRC crc(65, 0x04C11DB7);
    IoManagerPc ioManager(crc, 0x06, 0x02);
    ioManager.openSerialPort();

    std::vector<uint8_t> inputData = IoManagerPc::getBinaryInput();
    std::vector<uint8_t> outputData;

    std::thread transfer(&IoManagerPc::transfer2Way, &ioManager, std::ref(inputData), std::ref(outputData));

    transfer.join();

    ioManager.closeSerialPort();
    IoManagerPc::setBinaryOutput(outputData);

    return 0;
}