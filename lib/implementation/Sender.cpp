#include "../header/Sender.hpp"

Sender::Sender(AtomicQueue<uint8_t>* datastreamQueue_outgoing,AtomicQueue<InterthreadNotification>* notificationQueue_incoming)
    : datastreamQueue_outgoing(datastreamQueue_outgoing), notificationQueue_incoming(notificationQueue_incoming)
{ 
}

void processNotification(const InterthreadNotification &notification)
{

}

void Sender::send()
{
    // TODO: How to terminate program?
    while(true)
    {
        if (!notificationQueue_incoming->empty())
        {
            processNotification(notificationQueue_incoming->wait_and_pop());
        }
        else
        {
            // push data nibble into outQueue
        }
    }
}