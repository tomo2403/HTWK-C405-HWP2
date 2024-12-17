#include "../../header/Sender/SenderState_AwaitingResponse.hpp"
#include "../../header/Sender/SenderState_Sending.hpp"
#include "../../header/Sender/SenderState_Sending.hpp"
#include "../../header/ControlPacketAssembler.hpp"
#include <stdexcept>

// #include "../../header/ControlPacketAssembler.hpp"

SenderState_AwaitingResponse::SenderState_AwaitingResponse(Sender* sender, SenderResources* resources)
    : SenderState(sender, resources)
{
    timer.start();
}

void SenderState_AwaitingResponse::processNotification()
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
        resources->shutDownAsap = true;
        break;
    
    case InterthreadNotification::Type::OWN_PACKET_RECEIVED:
        // TODO: Check for correct packetId
        sender->setState(std::make_unique<SenderState_Sending>(sender, resources));
        resources->timeOutCounter = 0;
        break;
    
    case InterthreadNotification::Type::OWN_PACKET_RESEND:
        resources->nextPacketToBeSent_id--;
        sender->setState(std::make_unique<SenderState_Sending>(sender, resources));
        resources->timeOutCounter = 0;
        break;
    
    default:
        throw std::runtime_error("SenderState_Sending: Received illegal notification. It's unclear how to proceed.");
    }
}

void SenderState_AwaitingResponse::processDataQueueIsEmpty()
{
    const uint8_t secondsUntilTimeout = 10;
    if (timer.elapsed() >= secondsUntilTimeout)
    {
        // previous Packet again
        resources->nextPacketToBeSent_id--;
        resources->timeOutCounter++;

        if (resources->timeOutCounter >= 10)
        {
            throw std::runtime_error("SenderState_AwaitingResponse: Had 10 timeouts while awaiting response for sent packet.");
        }

        sender->setState(std::make_unique<SenderState_Sending>(sender, resources));
    }
}