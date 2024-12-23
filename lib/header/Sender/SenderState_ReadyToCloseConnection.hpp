#pragma once

#include "SenderState.hpp"

class SenderState_ReadyToCloseConnection final : public SenderState
{
public:
    SenderState_ReadyToCloseConnection(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;
};