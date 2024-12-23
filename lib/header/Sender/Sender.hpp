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

    bool running;

public:
    Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data);

    void setState(std::unique_ptr<SenderState> state);

    // To start in another thread
    void send();

    void shutDown();

    bool* getAtomicBoolean_pointer();
};
