#include "B15Receiver.hpp"
#include "../lib/lib.hpp"

#ifdef DEBUG_MODE
    B15Receiver::B15Receiver(B15Fake& drv, Decoder& decoder, Encoder& encoder)
#else
    B15Receiver::B15Receiver(B15F& drv, Decoder& decoder, Encoder& encoder)
#endif
    : drv(drv), decoder(decoder), encoder(encoder)
{
    this->previouslyReceivedNibble = 0xff;
    this->receivedData = std::vector<uint8_t>();

    decoder.addObserver(this);
}

void B15Receiver::beginBlockReceived(const BlockType &blockType)
{}

void B15Receiver::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
    if (dataVector.size() < 7)
    {
        encoder.interruptWithControlBlock(controlPanel.createControlBlock(Flags::RESEND, prevPacketID+1));
        return;
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

    if (blockType == BlockType::dataBlock)
    {
        if (!crcGenerator.validateCRC(ioManager::extractSubvector(dataVector, 0, dataVector.size()-4), crc))
        {
            encoder.interruptWithControlBlock(controlPanel.createControlBlock(Flags::RESEND, prevPacketID+1));
            return;
        }

        if (packetID != prevPacketID+1)
        {
            encoder.interruptWithControlBlock(controlPanel.createControlBlock(Flags::RESEND, prevPacketID+1));
            return;
        }

        prevPacketID = packetID;
        receivedData.reserve(dataVector.size() - 6);
        receivedData.insert(receivedData.end(), dataVector.begin()+2, dataVector.end()-4);
        return;
    }

    if (blockType == BlockType::controlBlock)
    {
        if (!crcGenerator.validateCRC(ioManager::extractSubvector(dataVector, 0, dataVector.size()-4), crc))
        {
            return;
        }

        if (dataVector.at(2) == Flags::TRANSFER_FINISHED)
        {
            ioManager::setBinaryOutput(receivedData);
            everythingReceived = true;
        }
        return;
    }
}

bool B15Receiver::isDifferentFromPrevious(const uint8_t &nibble)
{
    if (nibble != previouslyReceivedNibble)
    {
        previouslyReceivedNibble = nibble;
        return true;
    }

    return false;
}

void B15Receiver::receive()
{
    const uint8_t input = drv.getRegister(&PORTA) >> 4;

    if (!isDifferentFromPrevious(input))
    {
        return;
    }

    decoder.nextNibble(input);
}

bool B15Receiver::hasEverythingReceived()
{
    return everythingReceived;
}