#pragma once

#include "SenderState.hpp"

class SenderState_ReadyToCloseConnection : public SenderState
{
private:

public:
    SenderState_ReadyToCloseConnection(Sender* sender, SenderResources* resources);

    void processNotification();
    void processDataQueueIsEmpty();
};