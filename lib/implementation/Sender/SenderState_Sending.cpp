#include "../../header/Sender/SenderState_Sending.hpp"
#include "../../header/Sender/SenderState_ReadyToCloseConnection.hpp"
#include "../../header/Sender/SenderState_AwaitingResponse.hpp"
#include "../../header/ControlPacketAssembler.hpp"

#include <stdexcept>
#include <iostream>

SenderState_Sending::SenderState_Sending(Sender *sender, SenderResources *resources)
    : SenderState(sender, resources)
{
    if (!resources->globalTimer.running())
        resources->globalTimer.start();
}

void SenderState_Sending::processNotification()
{
    const InterthreadNotification notification = resources->notificationQueue_incoming->wait_and_pop();

    switch (notification.type)
    {
        case InterthreadNotification::Type::FOREIGN_PACKET_RECEIVED:
            resources->encoder.pushBlock(BlockType::controlBlock,
                                         ControlPacketAssembler::assemble(Flag::RECEIVED, notification.referencedPacket_id));
            break;

        case InterthreadNotification::Type::FOREIGN_PACKET_RESEND:
            resources->encoder.pushBlock(BlockType::controlBlock,
                                         ControlPacketAssembler::assemble(Flag::RESEND, notification.referencedPacket_id));
            break;

        case InterthreadNotification::Type::CLOSE_CONNECTION:
            resources->shutDownAsap = true;
            break;

        default:
            throw std::runtime_error("SenderState_Sending: Received illegal notification. It's unclear how to proceed.");
    }
}

void SenderState_Sending::processDataQueueIsEmpty()
{
    if (resources->dataPacketAssembler.packetDoesExist(resources->nextPacketToBeSent_id))
    {
        const std::vector<uint8_t> dataBlock = resources->dataPacketAssembler.getPacket(resources->nextPacketToBeSent_id);
        const auto callback = [&] { this->OnDataPacketSentCallback(); };
        resources->encoder.pushBlock(BlockType::dataBlock, dataBlock, callback);

        Logger(INFO, true) << "Sent packet: " << resources->nextPacketToBeSent_id << " | " << std::fixed << std::setprecision(4) <<
                resources->globalTimer.elapsed() << "s elapsed";
        resources->nextPacketToBeSent_id++;
    }
    else
    {
        sender->setState(std::make_unique<SenderState_ReadyToCloseConnection>(sender, resources));
    }
}

void SenderState_Sending::OnDataPacketSentCallback()
{
    sender->setState(std::make_unique<SenderState_AwaitingResponse>(sender, resources));
}
