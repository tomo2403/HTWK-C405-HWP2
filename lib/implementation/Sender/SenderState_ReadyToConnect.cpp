#include "../../header/Sender/SenderState_ReadyToConnect.hpp"
#include "../../header/Sender/SenderState_Sending.hpp"
#include <stdexcept>
#include "../../header/ControlPacketAssembler.hpp"
#include <iostream>

SenderState_ReadyToConnect::SenderState_ReadyToConnect(Sender* sender, SenderResources* resources)
    : SenderState(sender, resources)
{
}

void SenderState_ReadyToConnect::processNotification()
{
    const InterthreadNotification notification = resources->notificationQueue_incoming->wait_and_pop();

    if (notification.type != InterthreadNotification::Type::START_SENDING_DATA)
    {
        throw std::runtime_error("SenderState_ReadyToConnect: Received illegal notification. No connection was established, it's unclear how to proceed.");
    }

    sender->setState(std::make_unique<SenderState_Sending>(sender, resources));
}

void SenderState_ReadyToConnect::processDataQueueIsEmpty()
{
    // Optional: Füge eine kleine Verzögerung hinzu, um die CPU-Auslastung zu reduzieren
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const std::vector<uint8_t> controlBlock = ControlPacketAssembler::assemble(Flag::CONNECT, 0);
    resources->encoder.pushBlock(BlockType::controlBlock, controlBlock);
}