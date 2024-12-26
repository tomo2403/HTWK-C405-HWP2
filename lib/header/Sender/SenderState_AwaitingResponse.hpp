#pragma once

#include "SenderState.hpp"
#include "../Timer.hpp"

#define TIMEOUT_SECONDS 10

class SenderState_AwaitingResponse final : public SenderState
{
    Timer timer;

public:
    SenderState_AwaitingResponse(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;
};