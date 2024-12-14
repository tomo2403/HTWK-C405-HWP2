#pragma once

#include <vector>
#include "BlockType.hpp"

class IDecoderObserver
{
protected:
	~IDecoderObserver() = default;

public:
	virtual void beginBlockReceived(const BlockType &blockType) = 0;

	virtual void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) = 0;
};
