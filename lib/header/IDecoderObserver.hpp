#pragma once

#include "BlockType.hpp"

class IDecoderObserver
{
public:
    virtual void beginBlockReceived(const BlockType &blockType) = 0;
    virtual void endBlockReceived(const BlockType &blockType) = 0;
};
