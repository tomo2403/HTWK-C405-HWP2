#pragma once

#include "SenderState.hpp"
#include "SenderRessources.hpp"

#include "../AtomicQueue.hpp"
#include "../Encoder.hpp"
#include "../DataPacketAssembler.hpp"
#include "../Timer.hpp"
#include "../InterthreadNotification.hpp"
#include "../AtomicBoolean.hpp"

class SenderState;

class Sender
{
private:

    std::unique_ptr<SenderState> currentState;
    std::unique_ptr<SenderResources> resources;

    AtomicBoolean* running;

public:
    Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing,
           AtomicQueue<InterthreadNotification>* notificationQueue_incoming,
           AtomicBoolean* running,
           const std::vector<uint8_t> &data);

    void setState(std::unique_ptr<SenderState> state);

    // To start in another thread
    void send() const;

    void shutDown();
};
