#include "../../header/Sender/Sender.hpp"

#include <iomanip>

#include "../../header/Sender/SenderState_ReadyToConnect.hpp"

#include <memory>

SenderResources::SenderResources(AtomicQueue<uint8_t> *datastreamQueue_outgoing,
                                 AtomicQueue<InterthreadNotification> *notificationQueue_incoming, const std::vector<uint8_t> &data,
                                 ControlPanel *cp)
    : nextPacketToBeSent_id(0),
      datastreamQueue_outgoing(datastreamQueue_outgoing),
      notificationQueue_incoming(notificationQueue_incoming), dataPacketAssembler(data), cp(cp)
{
}

Sender::Sender(AtomicQueue<uint8_t> *datastreamQueue_outgoing, AtomicQueue<InterthreadNotification> *notificationQueue_incoming,
               std::atomic<bool> *running, const std::vector<uint8_t> &data, ControlPanel *cp)
    : running(running)
{
    resources = std::make_unique<SenderResources>(datastreamQueue_outgoing, notificationQueue_incoming, data, cp);

    currentState = std::make_unique<SenderState_ReadyToConnect>(this, resources.get());
}

void Sender::setState(std::unique_ptr<SenderState> state)
{
    currentState = std::move(state);
}

void Sender::send() const
{
    while (*running)
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

void Sender::shutDown() const
{
    resources->cp->setConnected(false);
    *running = false;
}
