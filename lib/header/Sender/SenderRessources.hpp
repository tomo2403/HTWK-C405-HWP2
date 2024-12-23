#pragma once

#include <cstdint>
#include "../AtomicQueue.hpp"
#include "../Encoder.hpp"
#include "../DataPacketAssembler.hpp"
#include "../Timer.hpp"
#include "../InterthreadNotification.hpp"

struct SenderResources
{
    uint8_t timeOutCounter{};
    uint32_t nextPacketToBeSent_id;
    bool shutDownAsap{};

    AtomicQueue<uint8_t>* datastreamQueue_outgoing;
    AtomicQueue<InterthreadNotification>* notificationQueue_incoming;

    Encoder encoder;
    DataPacketAssembler dataPacketAssembler;

    SenderResources(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data);
};