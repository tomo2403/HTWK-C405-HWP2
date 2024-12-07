#include "Serial.hpp"

void Serial::openPort()
{
    drv.setRegister(&DDRA, 0x0F);
}

int Serial::closePort() const
{
    Logger(DEBUG) << "Closing serial port...";
    return 0;
}

ssize_t Serial::writeByte(uint8_t data)
{
    std::lock_guard<std::mutex> lock(drvMutex);
    drv.setRegister(&PORTA, data);
    return 0;
}

ssize_t Serial::readByte(uint8_t &data)
{
    std::lock_guard<std::mutex> lock(drvMutex);
    data = drv.getRegister(&PINA) >> 4;
    return 0;
}

bool Serial::isDataAvailable()
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