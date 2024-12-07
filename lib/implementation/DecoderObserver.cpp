#include "../header/DecoderObserver.hpp"

void DecoderObserver::beginBlockReceived(const BlockType &blockType)
{
	cp->connect();
}

void DecoderObserver::endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector)
{
	if (dataVector.size() < 7)
		return;
	incomingQueue.push(std::make_pair(blockType, dataVector));
}

DecoderObserver::DecoderObserver(ControlPanel* cp) : cp(cp)
{
}