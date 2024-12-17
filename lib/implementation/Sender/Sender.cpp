#include "../../header/Sender/Sender.hpp"
#include "../../header/Sender/SenderState_ReadyToConnect.hpp"

#include <memory>

SenderResources::SenderResources(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data)
    : datastreamQueue_outgoing(datastreamQueue_outgoing), notificationQueue_incoming(notificationQueue_incoming), dataPacketAssembler(data)
{
}

Sender::Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing, AtomicQueue<InterthreadNotification>* notificationQueue_incoming, const std::vector<uint8_t> &data)
{
    // Initialisiere die Ressourcen
    resources = std::make_unique<SenderResources>(datastreamQueue_outgoing, notificationQueue_incoming, data);
    
    // Setze den Anfangszustand auf SenderState_ReadyToConnect
    currentState = std::make_unique<SenderState_ReadyToConnect>(this, resources.get());
}

void Sender::setState(std::unique_ptr<SenderState> state)
{
    currentState = std::move(state);
}

void Sender::send()
{
    while(true)
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