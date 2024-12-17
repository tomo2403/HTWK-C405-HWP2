#pragma once

#include "SenderState.hpp"

class SenderState_Sending : public SenderState
{
private:

public:
    SenderState_Sending(Sender* sender, SenderResources* resources);

    void processNotification();
    void processDataQueueIsEmpty();

    void OnDataPacketSentCallback();
};