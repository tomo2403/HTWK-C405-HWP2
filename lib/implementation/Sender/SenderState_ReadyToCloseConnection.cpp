#include "../../header/Sender/SenderState_ReadyToCloseConnection.hpp"
#include "../../header/ControlPacketAssembler.hpp"
#include <stdexcept>

SenderState_ReadyToCloseConnection::SenderState_ReadyToCloseConnection(Sender* sender, SenderResources* resources)
    : SenderState(sender, resources)
{
    if (resources->shutDownAsap)
    {
        sender->shutDown();
    }
}

void SenderState_ReadyToCloseConnection::processNotification()
{
    const InterthreadNotification notification = resources->notificationQueue_incoming->wait_and_pop();

    switch (notification.type)
    {
    case InterthreadNotification::Type::FOREIGN_PACKET_RECEIVED:
        resources->encoder.pushBlock(BlockType::controlBlock, ControlPacketAssembler::assemble(Flag::RECEIVED, notification.referencedPacket_id));
        break;

    case InterthreadNotification::Type::FOREIGN_PACKET_RESEND:
        resources->encoder.pushBlock(BlockType::controlBlock, ControlPacketAssembler::assemble(Flag::RESEND, notification.referencedPacket_id));
        break;

    case InterthreadNotification::Type::CLOSE_CONNECTION:
        sender->shutDown();
        break;
    
    default:
        throw std::runtime_error("SenderState_Sending: Received illegal notification. It's unclear how to proceed.");
    }
}

void SenderState_ReadyToCloseConnection::processDataQueueIsEmpty()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    const std::vector<uint8_t> controlBlock = ControlPacketAssembler::assemble(Flag::CLOSE_CONNECTION, 0);
    resources->encoder.pushBlock(BlockType::controlBlock, controlBlock);
}