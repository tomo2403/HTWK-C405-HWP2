#pragma once

#include "AtomicQueue.hpp"
#include "InterthreadNotification.hpp"

class Sender
{
private:
    static const uint8_t secondsUntilTimeout;

    AtomicQueue<uint8_t>* datastreamQueue_outgoing;
    AtomicQueue<InterthreadNotification>* notificationQueue_incoming;

    void processNotification(const InterthreadNotification &notification);

    void encoderCallback_onDataBlockSent();

public:
    Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming);

    // To start in another thread
    void send();
};
