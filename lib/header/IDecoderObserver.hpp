#pragma once

#include "BlockType.hpp"

class IDecoderObserver
{
public:
    virtual void beginBlockReceived(const BlockType &blockType) = 0;
    virtual void endBlockReceived(const BlockType &blockType, std::vector<uint8_t> dataVector) = 0;
};
