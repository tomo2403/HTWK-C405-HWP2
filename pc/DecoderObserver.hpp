#pragma once

#include "../lib/header/IDecoderObserver.hpp"
#include "Serial.hpp"

class DecoderObserver : public IDecoderObserver
{
public:
	void beginBlockReceived(const BlockType &blockType) override;

	void endBlockReceived(const BlockType &blockType) override;

	void iAmReadyReceived() override;

	void everythingSentReceived() override;

	void closeConnectionReceived() override;
};
