#pragma once

#include <vector>
#include <cstdint>

#include "../header/CRC.hpp"

class DataPacketAssembler
{
private:
    const static uint8_t dataBytesPerPacket;

    std::vector<uint8_t> rawData;

    void packetAssembly_addId(std::vector<uint8_t> &packet, const uint32_t &id);
    void packetAssembly_addData(std::vector<uint8_t> &packet, const uint32_t &id);
    void packetAssembly_addCrc(std::vector<uint8_t> &packet);

    
public:
    DataPacketAssembler(const std::vector<uint8_t> &rawData);

    std::vector<uint8_t> getPacket(const uint32_t &id);

    bool packetDoesExist(const uint32_t &id);
};
