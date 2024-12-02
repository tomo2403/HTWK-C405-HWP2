#include "B15Sender.hpp"

B15Sender::B15Sender(B15F& drv, Decoder& decoder, Encoder& encoder) : drv(drv), encoder(encoder), decoder(decoder)
{ }

void B15Sender::beginBlockReceived(const BlockType &blockType)
{

}

void B15Sender::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
    if (blockType != BlockType::controlBlock)
    {
        return; // Non of this methods business.
    }

    if (dataVector.size() != 7)
    {
        // error control package
        throw std::invalid_argument("Received Vector is of wrong size. This should be specially treated but it is currently not. BRUH");
    }

    // TODO: Potential bug, if msb is not received first.
    uint16_t packetID = 0;
    packetID |= (static_cast<uint16_t>(dataVector.at(0)) << 8);
    packetID |= (static_cast<uint16_t>(dataVector.at(1)));

    uint32_t crc = 0;
    crc |= (static_cast<uint32_t>(dataVector.at(dataVector.size() - 4)) << 24);
    crc |= (static_cast<uint32_t>(dataVector.at(dataVector.size() - 3)) << 16);
    crc |= (static_cast<uint32_t>(dataVector.at(dataVector.size() - 2)) << 8);
    crc |= (static_cast<uint32_t>(dataVector.at(dataVector.size() - 1)) << 0);

    if (!crcGenerator.validateCRC(ioManager::extractSubvector(dataVector, 0, dataVector.size()-4), crc))
    {
        return; // Control Package is broken - do nothing and wait for sender to send again.
    }
}

void B15Sender::send()
{
    if (encoder.hasData())
    {
        drv.setRegister(&PORTA, encoder.nextNibble());
    }
    else
    {

    }
    
}