#include "IoManagerB15F.hpp"
#include <b15f/b15f.h>

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

/*
int main()
{
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0xff);
    
    while (true)
    {
        drv.setRegister(&PORTA, 0xff);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        drv.setRegister(&PORTA, 0x00);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
*/