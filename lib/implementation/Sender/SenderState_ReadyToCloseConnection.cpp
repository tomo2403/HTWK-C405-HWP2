#include "../../header/Sender/SenderState_ReadyToCloseConnection.hpp"
#include "../../header/ControlPacketAssembler.hpp"
#include <stdexcept>
#include <thread>

#define minNumberOfPacketsToSent 2

SenderState_ReadyToCloseConnection::SenderState_ReadyToCloseConnection(Sender *sender, SenderResources *resources)
    : SenderState(sender, resources), numberOfCloseConnectionPacketsSent(0)
{
    if (resources->globalTimer.running())
        resources->globalTimer.stop();

    timeSinceLastReceivedPacket_timer.start();
}

void SenderState_ReadyToCloseConnection::processNotification()
{
    const InterthreadNotification notification = resources->notificationQueue_incoming->wait_and_pop();
    timeSinceLastReceivedPacket_timer.start();

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
            if (numberOfCloseConnectionPacketsSent >= minNumberOfPacketsToSent)
            {
                sender->shutDown();
            }
            else
            {
                resources->shutDownAsap = true;
                resources->cp->finishReceiving();
            }
            break;

        default:
            throw std::runtime_error("SenderState_Sending: Received illegal notification. It's unclear how to proceed.");
    }
}

void SenderState_ReadyToCloseConnection::processDataQueueIsEmpty()
{
    resources->cp->finishSending();
    // Shutdown is already scheduled and specified number of closeConnectionPackets was sent.
    if (resources->shutDownAsap && numberOfCloseConnectionPacketsSent >= minNumberOfPacketsToSent)
    {
        sender->shutDown();
    }

    // The time since the last received notification, and therefore control-Packet, exceeds 60s and
    // no shutDown is scheduled yet.
    if (timeSinceLastReceivedPacket_timer.elapsed() >= 60 && !resources->shutDownAsap)
    {
        sender->shutDown();
        Logger(WARNING) <<
                "Waiting to close the connection but did not receive any packets for 60 seconds. Closing connection due to timeout.\n";
    }

    // Ensures CloseConnection packets are sent at least 3s apart.
    if (timeSinceLastCloseConnectionPacket_timer.elapsed() >= 3 || !timeSinceLastCloseConnectionPacket_timer.running())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        const std::vector<uint8_t> controlBlock = ControlPacketAssembler::assemble(Flag::CLOSE_CONNECTION, 0);
        resources->encoder.pushBlock(BlockType::controlBlock, controlBlock);
        numberOfCloseConnectionPacketsSent++;
        timeSinceLastCloseConnectionPacket_timer.start();
    }
}
