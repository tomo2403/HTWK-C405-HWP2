#pragma once

#include "SenderState.hpp"
#include "SenderRessources.hpp"

#include "../AtomicQueue.hpp"
#include "../DataPacketAssembler.hpp"
#include "../InterthreadNotification.hpp"
#include "../Logger.hpp"

class SenderState;

class Sender
{

    std::unique_ptr<SenderState> currentState;
    std::unique_ptr<SenderResources> resources;

    std::atomic<bool>* running;

public:
    Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing,
           AtomicQueue<InterthreadNotification>* notificationQueue_incoming,
           std::atomic<bool>* running,
           const std::vector<uint8_t> &data);

    void setState(std::unique_ptr<SenderState> state);

    // To start in another thread
    void send() const;

    void shutDown() const;
};
