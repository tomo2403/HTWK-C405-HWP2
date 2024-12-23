#pragma once

#include "SenderState.hpp"

class SenderState_AwaitingResponse final : public SenderState
{
private:
    Timer timer;

public:
    SenderState_AwaitingResponse(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;
};