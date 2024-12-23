#pragma once

#include "SenderState.hpp"

class SenderState_ReadyToConnect final : public SenderState
{
public:
    SenderState_ReadyToConnect(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;
};