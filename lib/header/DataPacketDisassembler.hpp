#include "CRC.hpp"

class DataPacketDisassembler
{
private:
    const static uint8_t dataBytesPerPacket;
    std::vector<uint8_t> rawData;

    uint16_t packetDisassembly_getId(const std::vector<uint8_t> &packet);
    std::vector<uint8_t> packetDisassembly_getData(const std::vector<uint8_t> &packet);
    uint32_t packetDisassembly_getCrc(const std::vector<uint8_t> &packet);

public:
    DataPacketDisassembler();

    bool processPacket(const std::vector<uint8_t> &packet, const uint16_t &expectedId);

    std::vector<uint8_t> getData();
};