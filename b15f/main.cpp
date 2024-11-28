#include <b15f/b15f.h>
#include "B15Receiver.hpp"

int main()
{
	B15F& drv = B15F::getInstance();

	// 7-4 inbound, 3-0 outbound
	drv.setRegister(&DDRA, 0x0F);
	
	B15Receiver receiver = B15Receiver(drv);

	while(true)
	{
		receiver.receive();
	}

	return 0;
}