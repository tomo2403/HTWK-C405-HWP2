#include "B15Receiver.hpp"

B15Receiver::B15Receiver(B15F& drv) : drv(drv)
{
    this->packetCounter = 0;
    this->decoder = Decoder();

    decoder.addObserver(this);
}

void B15Receiver::beginBlockReceived(const BlockType &blockType)
{}

void B15Receiver::endBlockReceived(const BlockType &blockType)
{}

void B15Receiver::receive()
{
}