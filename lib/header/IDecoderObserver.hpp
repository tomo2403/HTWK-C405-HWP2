#pragma once

#include <vector>
#include <cstdint>
#include "BlockType.hpp"

class IDecoderObserver
{
public:
    virtual void beginBlockReceived(const BlockType &blockType) = 0;
    virtual void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) = 0;
};
