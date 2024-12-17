#pragma once

#include "SenderState.hpp"

class SenderState_AwaitingResponse : public SenderState
{
private:
    Timer timer;

public:
    SenderState_AwaitingResponse(Sender* sender, SenderResources* resources);

    void processNotification();
    void processDataQueueIsEmpty();
};