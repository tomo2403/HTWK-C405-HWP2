#pragma once

#include "SenderState.hpp"

class SenderState_ReadyToConnect : public SenderState
{
private:

public:
    SenderState_ReadyToConnect(Sender* sender, SenderResources* resources);

    void processNotification();
    void processDataQueueIsEmpty();
};