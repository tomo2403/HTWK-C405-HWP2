#pragma once

#include <vector>
#include <cstdint>

class DataPacketAssembler
{
    const static uint8_t dataBytesPerPacket;

    std::vector<uint8_t> rawData;

    static void packetAssembly_addId(std::vector<uint8_t> &packet, const uint32_t &id);
    void packetAssembly_addData(std::vector<uint8_t> &packet, const uint32_t &id) const;

    static void packetAssembly_addCrc(std::vector<uint8_t> &packet);

    
public:
    explicit DataPacketAssembler(const std::vector<uint8_t> &rawData);

    [[nodiscard]] std::vector<uint8_t> getPacket(const uint32_t &id) const;

    [[nodiscard]] bool packetDoesExist(const uint32_t &id) const;
};
