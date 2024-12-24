#include "../header/ControlPacketAssembler.hpp"

void ControlPacketAssembler::packetAssembly_addId(std::vector<uint8_t> &packet, const uint32_t &id)
{
    packet.reserve(3);
    packet.push_back(static_cast<uint8_t>(id >> 16 & 0xFF));
    packet.push_back(static_cast<uint8_t>(id >> 8 & 0xFF));
    packet.push_back(static_cast<uint8_t>(id >> 0 & 0xFF));
}

void ControlPacketAssembler::packetAssembly_addFlags(std::vector<uint8_t> &packet, const uint8_t &flags)
{
    packet.push_back(flags);
}

void ControlPacketAssembler::packetAssembly_addCrc(std::vector<uint8_t> &packet)
{
    const uint32_t crc = CRC::calculateCRC(packet);
    
    packet.reserve(4);

    // 'i' is an element of {3, 2, 1, 0}
    for (int8_t i = 3; i >= 0; --i)
    {
        packet.push_back(static_cast<uint8_t>(crc >> i*8));
    }
}

std::vector<uint8_t> ControlPacketAssembler::assemble(const uint8_t &flag, const uint32_t &packetId)
{
    std::vector<uint8_t> packet;
    packetAssembly_addId(packet, packetId);
    packetAssembly_addFlags(packet, flag);
    packetAssembly_addCrc(packet);

    return packet;
}
