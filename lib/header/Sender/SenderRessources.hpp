#pragma once

#include <cstdint>
#include "../AtomicQueue.hpp"
#include "../Encoder.hpp"
#include "../DataPacketAssembler.hpp"
#include "../Timer.hpp"
#include "../InterthreadNotification.hpp"

struct SenderResources
{
    static const uint8_t secondsUntilTimeout;

    uint16_t nextPacketToBeSent_id;
    bool shutDownAsap;

    AtomicQueue<uint8_t>* datastreamQueue_outgoing;
    AtomicQueue<InterthreadNotification>* notificationQueue_incoming;

    Encoder encoder;
    DataPacketAssembler dataPacketAssembler;
    Timer timer;

    SenderResources(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data);
};