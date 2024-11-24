#include "IoManagerB15F.hpp"

IoManagerB15F::IoManagerB15F(CRC crc, uint8_t outboundChannel, uint8_t inboundChannel)
    : IoManager(crc, outboundChannel, inboundChannel)
{
    this->prevNibbleOnChannelZero = drv.getRegister(&PINA) & 0x0F;
    this->prevNibbleOnChannelFour = drv.getRegister(&PINA) & 0xF0;
}

void IoManagerB15F::sendPacket(const StreamPacket &sp)
{
    Encoder encoder = Encoder(sp.data);

    switch (sp.channel)
    {
    case 0:
        // bit 7-4 unchanged, bit 3-0 output
        drv.setRegister(&DDRA,  (drv.getRegister(&DDRA) & 0xF0));
        break;
    case 4:
        // bit 7-4 output, bit 3-0 unchanged
        drv.setRegister(&DDRA,  (drv.getRegister(&DDRA) & 0x0F));
        break;
    }

    while (encoder.hasData())
    {
        drv.setRegister(&PORTA, encoder.nextNibble() << sp.channel);
    }
}

bool IoManagerB15F::pinHasChanged(const uint8_t &channel)
{
    uint8_t prevNibble = channel == 0 ? prevNibbleOnChannelZero : prevNibbleOnChannelFour;

    const uint8_t currentNibble = drv.getRegister(&DDRA) & (0x0F << channel);
    if (prevNibble == currentNibble)
    {
        return false;
    }

    prevNibble = currentNibble;
    return true;
}

void IoManagerB15F::receivePacket(StreamPacket &sp)
{
    std::vector<uint8_t> encodedData = std::vector<uint8_t>();
    Decoder decoder = Decoder(encodedData);

    const uint8_t channelToReadFrom = awaitingResponse ? inboundChannel : outboundChannel;

    while(!decoder.hasData())
    {
        if (pinHasChanged(channelToReadFrom))
        {
            decoder.nextNibble(drv.getRegister(&PINA) & (0x0F << channelToReadFrom));
        }
    }

    sp.channel = channelToReadFrom;
    sp.dataLength = encodedData.size();
    sp.data = encodedData;
}

void IoManagerB15F::getContinuesInput()
{
    std::cerr << "Test text" << std::endl;
    drv.setRegister(&DDRA,  (drv.getRegister(&DDRA) & 0x0F));

    while (true)
    {
        StreamPacket sp{};
        receivePacket(sp);
        incomingQueue.push(sp);
    }
}