#pragma once

#include "SenderState.hpp"

class SenderState_ReadyToCloseConnection final : public SenderState
{
private:
    Timer timeSinceLastReceivedPacket_timer;

public:
    SenderState_ReadyToCloseConnection(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;
};