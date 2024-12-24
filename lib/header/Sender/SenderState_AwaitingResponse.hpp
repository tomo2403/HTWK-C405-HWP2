#pragma once

#include "SenderState.hpp"
#include "../Timer.hpp"

class SenderState_AwaitingResponse final : public SenderState
{
    Timer timer;

public:
    SenderState_AwaitingResponse(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;
};