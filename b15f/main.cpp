#include <b15f/b15f.h>
// #include "B15Receiver.hpp"
#include "../lib/header/Encoder.hpp"
#include "../lib/header/ControlPanel.hpp"
#include "../lib/lib.hpp"

/*
int main()
{
	B15F& drv = B15F::getInstance();
	Decoder decoder = Decoder();
	Encoder encoder = Encoder();

	// 7-4 inbound, 3-0 outbound
	drv.setRegister(&DDRA, 0x0F);
	
	B15Receiver receiver = B15Receiver(drv, decoder, encoder);

	while(true)
	{
		receiver.receive();
	}

	return 0;
}
*/

int main()
{
	B15F& drv = B15F::getInstance();
	drv.setRegister(&DDRA, 0x0F);
	ControlPanel controlPanel;
	Encoder encoder = Encoder();
	CRC crcPanel = CRC(0x00, 0x00);
	std::vector<uint8_t> connectPacket = controlPanel.createControlBlock(Flags::CONNECT, 0);

	crcPanel.attachCRC(connectPacket);
	while (true)
	{
		encoder.interruptWithControlBlock(connectPacket);
		
		while(encoder.hasData())
		{
			uint8_t nN = encoder.nextNibble();
			drv.setRegister(&PORTA, nN);
		}
	}
	
}