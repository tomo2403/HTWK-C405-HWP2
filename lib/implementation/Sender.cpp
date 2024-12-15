#include "../header/Sender.hpp"

Sender::Sender(AtomicQueue<uint8_t>* outgoingQueue)
    : outgoingQueue(outgoingQueue)
{ 
}