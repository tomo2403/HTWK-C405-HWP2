#pragma once

#include "AtomicQueue.hpp"

class Sender
{
private:

    AtomicQueue<uint8_t>* outgoingQueue;;


public:
    Sender(AtomicQueue<uint8_t>* outgoingQueue);

    // To start in another thread
    void send();
};
