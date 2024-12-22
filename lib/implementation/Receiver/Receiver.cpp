#include "../../header/Receiver/Receiver.hpp"
#include <iostream>

Receiver::Receiver(AtomicQueue<uint8_t>* datastreamQueue_incoming, AtomicQueue<InterthreadNotification>* notificationQueue_outgoing)
    : datastreamQueue_incoming(datastreamQueue_incoming), notificationQueue_outgoing(notificationQueue_outgoing), connectionEstablished(false)
{
    decoder.addObserver(this);
    controlPacketDisassembler.addObserver(this);
}

void Receiver::receive()
{
    while (true)
    {
        std::optional<uint8_t> optionalValue = datastreamQueue_incoming->try_pop();
        
        if (optionalValue) {
            std::cout << std::hex << (int) *optionalValue << std::endl;
            decoder.nextNibble(*optionalValue);
        }
    }
}



void Receiver::beginBlockReceived(const BlockType &blockType)
{
}

void Receiver::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
    if (blockType == BlockType::dataBlock)
    {
        dataBlockReceived(dataVector);
    }
    else
    {
        controlBlockReceived(dataVector);
    }
}

void Receiver::controlBlockReceived(const std::vector<uint8_t> &dataVector)
{
    // No further processing here, because processPacket() will callback via observer,
    // methods 'this' is a observer of 'controlPacketDisassembler'.
    controlPacketDisassembler.processPacket(dataVector);
}

void Receiver::dataBlockReceived(const std::vector<uint8_t> &dataVector)
{
    if(dataPacketDisassembler.processPacket(dataVector, nextPacketToBeReceived_id))
    {
        nextPacketToBeReceived_id++;
        notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::FOREIGN_PACKET_RECEIVED, nextPacketToBeReceived_id));
    }
    else
    {
        notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::FOREIGN_PACKET_RESEND, nextPacketToBeReceived_id));
    }
}

void Receiver::on_received_received(const uint16_t &id)
{
    notificationQueue_outgoing->push(InterthreadNotification(InterthreadNotification::Type::OWN_PACKET_RECEIVED, id));
}

void Receiver::on_resend_received(const uint16_t &id)
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