#pragma once

#include "SenderState.hpp"

class SenderState_Sending final : public SenderState
{
public:
    SenderState_Sending(Sender* sender, SenderResources* resources);

    void processNotification() override;
    void processDataQueueIsEmpty() override;

    void OnDataPacketSentCallback();
};