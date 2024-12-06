#include "b15global.hpp"
#ifndef DEBUG_MODE
    #include <b15f/b15f.h>
#endif
#include "../lib/lib.hpp"
#include "B15Receiver.hpp"
#include "B15Sender.hpp"

int main()
{
	Decoder decoder = Decoder();
	Encoder encoder = Encoder();

	#ifdef DEBUG_MODE
		std::vector<uint8_t> incommingData =
		{};

		std::vector<uint8_t>  outgoingData =
		{};


		B15Fake drv = B15Fake(incommingData);
		B15Sender sender = B15Sender(drv, decoder, encoder, outgoingData);
	#else
		B15F& drv = B15F::getInstance();
		B15Sender sender = B15Sender(drv, decoder, encoder, ioManager::getBinaryInput());
	#endif

	// 7-4 inbound, 3-0 outbound
	drv.setRegister(&DDRA, 0x0F);
	
	B15Receiver receiver = B15Receiver(drv, decoder, encoder);

	while(true)
	{
		receiver.receive();
		sender.send();
	}

	return 0;
}

/*
int main()
{
	B15F& drv = B15F::getInstance();
	drv.setRegister(&DDRA, 0x0F);
	ControlPanel controlPanel;
	Encoder encoder = Encoder();
	CRC crcPanel = CRC(0x04c11db7, 0xfffffffff);
	std::vector<uint8_t> connectPacket = controlPanel.createControlBlock(Flags::CONNECT, 0);

	crcPanel.attachCRC(connectPacket);
	while (true)
	{
		encoder.interruptWithControlBlock(connectPacket);
		
		while(encoder.hasData())
		{
			uint8_t nN = encoder.nextNibble();
			// drv.delay_ms(5000);
			drv.setRegister(&PORTA, nN);
		}
	}
	
}
*/