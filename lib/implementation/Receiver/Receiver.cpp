#include "../../header/Receiver/Receiver.hpp"
#include "../../lib.hpp"
#include "../../header/Logger.hpp"

Receiver::Receiver(AtomicQueue<uint8_t> *datastreamQueue_incoming, AtomicQueue<InterthreadNotification> *notificationQueue_outgoing,
                   std::atomic<bool> *running)
    : datastreamQueue_incoming(datastreamQueue_incoming), notificationQueue_outgoing(notificationQueue_outgoing), running(running),
      connectionEstablished(false), nextPacketToBeReceived_id(0)
{
    decoder.addObserver(this);
    controlPacketDisassembler.addObserver(this);
}

std::vector<uint8_t> Receiver::getOutputData()
{
    return dataPacketDisassembler.getData();
}

void Receiver::receive()
{
    while (*running)
    {
        if (!datastreamQueue_incoming->empty())
        {
            decoder.nextNibble(datastreamQueue_incoming->wait_and_pop());
        }
    }
}

void Receiver::beginBlockReceived(const BlockType &blockType)
{
}

void Receiver::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
    Logger(DEBUG) << "Received block of type " << blockType;
    if (blockType == BlockType::dataBlock)
    {
        dataBlockReceived(dataVector);
    }
    else
    {
        controlBlockReceived(dataVector);
    }
}

void Receiver::controlBlockReceived(const std::vector<uint8_t> &dataVector) const
{
    // No further processing here, because processPacket() will callback via observer,
    // methods 'this' is an observer of 'controlPacketDisassembler'.
    controlPacketDisassembler.processPacket(dataVector);
}

void Receiver::dataBlockReceived(const std::vector<uint8_t> &dataVector)
{
    if (dataPacketDisassembler.processPacket(dataVector, nextPacketToBeReceived_id))
    {
        notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::FOREIGN_PACKET_RECEIVED,
                                                                 nextPacketToBeReceived_id));
        nextPacketToBeReceived_id++;
    }
    else
    {
        notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::FOREIGN_PACKET_RESEND,
                                                                 nextPacketToBeReceived_id));
    }
}

void Receiver::on_received_received(const uint32_t &id)
{
    notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::OWN_PACKET_RECEIVED, id));
}

void Receiver::on_resend_received(const uint32_t &id)
{
    notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::OWN_PACKET_RESEND, id));
}

void Receiver::on_transferFinished_received()
{
    notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::CLOSE_CONNECTION));
}

void Receiver::on_closeConnection_received()
{
    notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::CLOSE_CONNECTION));
}

void Receiver::on_connect_received()
{
    if (!connectionEstablished)
    {
        notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::START_SENDING_DATA));
        connectionEstablished = true;
    }
}

void Receiver::on_packetCorrupt()
{
    // Idea: Tell sender about corrupt packet. Let him decide how to act.
    throw std::runtime_error("Corrupt control packet received.");
}
