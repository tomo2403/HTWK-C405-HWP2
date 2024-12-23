#pragma once

#include "Sender.hpp"
#include "SenderRessources.hpp"

class Sender;

class SenderState
{
protected:
    Sender* sender;
    SenderResources* resources;

public:
    virtual ~SenderState() = default;

    SenderState(Sender* sender, SenderResources* resources);
    SenderState() = delete;

    virtual void processNotification() = 0;
    virtual void processDataQueueIsEmpty() = 0;
};

