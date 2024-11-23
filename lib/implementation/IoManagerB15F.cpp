#include "../header/IoManagerB15F.hpp"

IoManagerB15F::IoManagerB15F(CRC crc, uint8_t outboundChannel) : IoManager(crc, outboundChannel)
{

}

void IoManagerB15F::sendPacket(const StreamPacket &sp)
{

}

bool IoManagerB15F::checkResponse()
{
	return true;
}

void IoManagerB15F::sendResponse(uint8_t channel, u_long packetIndex, bool success)
{

}
