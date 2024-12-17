#pragma once

#include "Sender.hpp"
#include "SenderRessources.hpp"

class Sender;

class SenderState
{
private:
    SenderState() = delete;

protected:
    Sender* sender;
    SenderResources* resources;

public:
    SenderState(Sender* sender, SenderResources* resources);

    virtual void processNotification() = 0;
    virtual void processDataQueueIsEmpty() = 0;
};

