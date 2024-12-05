#include "B15Sender.hpp"

B15Sender::B15Sender(B15F& drv, Decoder& decoder, Encoder& encoder,const std::vector<uint8_t> &rawDataToSend)
    : drv(drv), encoder(encoder), decoder(decoder), rawDataToSend(rawDataToSend)
{ }

void B15Sender::beginBlockReceived(const BlockType &blockType)
{

}

std::vector<uint8_t> B15Sender::getPackageById(const uint16_t &id)
{
    const std::vector<uint8_t> rawData = getRawDataById(id);
    const uint32_t crc = crcGenerator.calculateCRC(rawData);

    const uint8_t packageSize_Byte = rawData.size() + 6;
    std::vector<uint8_t> dataPackage = std::vector<uint8_t>(packageSize_Byte);

    dataPackage[0] = static_cast<uint8_t>(id >> 8);
    dataPackage[1] = static_cast<uint8_t>(id & 0x00FF);

    dataPackage.insert(dataPackage.begin() + 2, rawData.begin(), rawData.end());

    dataPackage[packageSize_Byte-4] = static_cast<uint8_t>((crc >> 24) & 0xFF);
    dataPackage[packageSize_Byte-3] = static_cast<uint8_t>((crc >> 16) & 0xFF);
    dataPackage[packageSize_Byte-2] = static_cast<uint8_t>((crc >> 8) & 0xFF);
    dataPackage[packageSize_Byte-1] = static_cast<uint8_t>((crc >> 0) & 0xFF);

    return dataPackage;

}

std::vector<uint8_t> B15Sender::getRawDataById(const uint16_t &id)
{
    if (id * packetSize + packetSize > rawDataToSend.size())
    {
        return ioManager::extractSubvector(rawDataToSend, id * packetSize, rawDataToSend.size() - id * packetSize);
    }

    return ioManager::extractSubvector(rawDataToSend, id * packetSize, packetSize);
}

void B15Sender::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
    if (blockType != BlockType::controlBlock)
    {
        return; // Non of this methods business.
    }

    if (dataVector.size() != 7 && connectionEstablished)
    {
        // resend package;
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
        encoder.inputDataBlock(getPackageById(prevPacketID));
    }

    if (!connectionEstablished && dataVector.at(3) == Flags::CONNECT)
    {
        connectionEstablished = true;
        encoder.inputDataBlock(getPackageById(++prevPacketID));
    }

    if (dataVector.at(3) == Flags::RESEND)
    {
        encoder.inputDataBlock(getPackageById(prevPacketID));
    }

    if (dataVector.at(3) == Flags::RECEIVED)
    {
        encoder.inputDataBlock(getPackageById(++prevPacketID));
    }
}

void B15Sender::send()
{
    if (encoder.hasData())
    {
        drv.setRegister(&PORTA, encoder.nextNibble());
        timer.start();
    }
    else if (connectionEstablished && timer.elapsedSeconds() > 5)
    {
        encoder.inputDataBlock(getPackageById(prevPacketID));
    }
}