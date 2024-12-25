#pragma once

#include <vector>

#include "IControlPacketDisassemblerObserver.hpp"

class ControlPacketDisassembler
{

    std::vector<IControlPacketDisassemblerObserver *> observers;

    static uint32_t packetDisassembly_getId(const std::vector<uint8_t> &packet);

    static uint32_t packetDisassembly_getCrc(const std::vector<uint8_t> &packet);
    void packetDisassembly_processFlags(const std::vector<uint8_t> &packet, const uint32_t &id) const;

public:

    void addObserver(IControlPacketDisassemblerObserver *observer);

	void removeObserver(IControlPacketDisassemblerObserver *observer);

    void processPacket(const std::vector<uint8_t> &packet) const;
};