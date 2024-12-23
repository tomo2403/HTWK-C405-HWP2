#include "../header/DataPacketDisassembler.hpp"

#include <stdexcept>

#include "../utilities.hpp"

const uint8_t DataPacketDisassembler::dataBytesPerPacket = 64;

DataPacketDisassembler::DataPacketDisassembler()
    : rawData(std::vector<uint8_t>())
{
}

uint32_t DataPacketDisassembler::packetDisassembly_getId(const std::vector<uint8_t> &packet)
{
    uint32_t id = 0x00;
    id = (id | packet.at(0)) << 8;
    id = (id | packet.at(1)) << 8;
	id = (id | packet.at(2)) << 0;

    return id;
}

std::vector<uint8_t> DataPacketDisassembler::packetDisassembly_getData(const std::vector<uint8_t> &packet)
{
    return utilities::extractSubvector(packet, 3, packet.size()-7);
}

uint32_t DataPacketDisassembler::packetDisassembly_getCrc(const std::vector<uint8_t> &packet)
{
    uint32_t crc = 0x00;
    crc = (crc | packet.at(packet.size()-4)) << 8;
    crc = (crc | packet.at(packet.size()-3)) << 8;
    crc = (crc | packet.at(packet.size()-2)) << 8;
    crc = (crc | packet.at(packet.size()-1)) << 0;

    return crc;
}

bool DataPacketDisassembler::processPacket(const std::vector<uint8_t> &packet, const uint32_t &expectedId)
{
    if (packet.size() < 8)
    {
        throw std::invalid_argument("DataPacketDisassembler: Packets cannot have a size < 7.");
    }

    uint32_t packetId = packetDisassembly_getId(packet);
    std::vector<uint8_t> data = packetDisassembly_getData(packet);
    uint32_t crc = packetDisassembly_getCrc(packet);

    if (packetId != expectedId)
    {
        return false;
    }

    // TODO: More elegant crc solution
    if (!CRC::validateCRC(utilities::extractSubvector(packet, 0, packet.size()-4), crc))
    {
        return false;
    }

    // Possible issue with rawData.begin() + dataBytesPerPacket * packetId.
    rawData.insert(rawData.begin() + dataBytesPerPacket * packetId, data.begin(), data.end());
    return true;
}

std::vector<uint8_t> DataPacketDisassembler::getData()
{
    return rawData;
}
