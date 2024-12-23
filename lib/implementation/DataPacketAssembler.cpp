#include "../header/DataPacketAssembler.hpp"

#include <stdexcept>
#include <cmath>

#include "../utilities.hpp"

const uint8_t DataPacketAssembler::dataBytesPerPacket = 64;

void DataPacketAssembler::packetAssembly_addId(std::vector<uint8_t> &packet, const uint16_t &id)
{
    packet.reserve(2);
    packet.push_back(static_cast<uint8_t>(id >> 8));
    packet.push_back(static_cast<uint8_t>(id & 0xFF));
}

void DataPacketAssembler::packetAssembly_addData(std::vector<uint8_t> &packet, const uint16_t &id)
{
    const size_t packetDataStartIndex = id * dataBytesPerPacket;
    const bool hasSufficientData = packetDataStartIndex + dataBytesPerPacket <= rawData.size();
    const uint8_t packetSize = hasSufficientData ? dataBytesPerPacket : rawData.size() - packetDataStartIndex;

    packet.reserve(packetSize);

    std::vector<uint8_t> data = utilities::extractSubvector(rawData, packetDataStartIndex, packetSize);
    packet.insert(packet.end(), data.begin(), data.end());
}

void DataPacketAssembler::packetAssembly_addCrc(std::vector<uint8_t> &packet)
{
    const uint32_t crc = CRC::calculateCRC(packet);
    
    packet.reserve(4);

    // 'i' is an element of {3, 2, 1, 0}
    for (int8_t i = 3; i >= 0; --i)
    {
        packet.push_back(static_cast<uint8_t>(crc >> (i*8)));
    }
}

DataPacketAssembler::DataPacketAssembler(const std::vector<uint8_t> &rawData)
    : rawData(rawData)
{
    if (rawData.size() == 0)
    {
        throw std::invalid_argument("DataPacketAssembler: Raw-Data-Vector must not be empty.");
    }
}

std::vector<uint8_t> DataPacketAssembler::getPacket(const uint16_t &id)
{
    std::vector<uint8_t> packet = std::vector<uint8_t>();
    packetAssembly_addId(packet, id);
    packetAssembly_addData(packet, id);
    packetAssembly_addCrc(packet);

    return packet;
}

bool DataPacketAssembler::packetDoesExist(const uint16_t &id)
{
    const size_t availableDatePackets = std::ceil(rawData.size() / static_cast<double>(dataBytesPerPacket));
    return availableDatePackets > id;
}