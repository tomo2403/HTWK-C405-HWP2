#pragma once

#include "CRC.hpp"

class DataPacketDisassembler
{
    const static uint8_t dataBytesPerPacket;
    std::vector<uint8_t> rawData;

    static uint32_t packetDisassembly_getId(const std::vector<uint8_t> &packet);

    static std::vector<uint8_t> packetDisassembly_getData(const std::vector<uint8_t> &packet);

    static uint32_t packetDisassembly_getCrc(const std::vector<uint8_t> &packet);

public:
    DataPacketDisassembler();

    bool processPacket(const std::vector<uint8_t> &packet, const uint32_t &expectedId);

    std::vector<uint8_t> getData();
};