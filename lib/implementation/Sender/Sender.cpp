#include "../../header/Sender/Sender.hpp"
#include "../../header/Sender/SenderState_ReadyToConnect.hpp"

#include <memory>

SenderResources::SenderResources(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data)
    : nextPacketToBeSent_id(0),
      datastreamQueue_outgoing(datastreamQueue_outgoing),
      notificationQueue_incoming(notificationQueue_incoming), dataPacketAssembler(data)
{
}

Sender::Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data)
{
    resources = std::make_unique<SenderResources>(datastreamQueue_outgoing, notificationQueue_incoming, data);
    
    currentState = std::make_unique<SenderState_ReadyToConnect>(this, resources.get());

    running = true;
}

void Sender::setState(std::unique_ptr<SenderState> state)
{
    currentState = std::move(state);
}

void Sender::send() const
{
    while(running)
    {
        if (!resources->notificationQueue_incoming->empty())
        {
            currentState->processNotification();
        }
        else if (resources->encoder.hasData())
        {
            resources->datastreamQueue_outgoing->push(resources->encoder.nextNibble());
        }
        else
        {
            currentState->processDataQueueIsEmpty();
        }
    }
}

void Sender::shutDown()
{
    running = false;
}

bool* Sender::getAtomicBoolean_pointer()
{
    return &running;
}