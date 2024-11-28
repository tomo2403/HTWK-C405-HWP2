#include "B15Receiver.hpp"

B15Receiver::B15Receiver(B15F& drv) : drv(drv)
{
    this->packetCounter = 0;
    this->decoder = Decoder();
    this->previouslyReceivedNibble = 0xff;
    this->receivedData = std::vector<uint8_t>();

    decoder.addObserver(this);
}

void B15Receiver::beginBlockReceived(const BlockType &blockType)
{}

void B15Receiver::endBlockReceived(const BlockType &blockType)
{
    const std::vector<uint8_t> block = decoder.getDataVector();

    // TODO: Potential bug, if msb is not received first.
    const uint16_t packetID = 0 | block.at(0) << 8 | block.at(1);

    const uint32_t crc = 0 | block.at(block.size() - 4) << 24 | block.at(block.size() - 4) << 24 | block.at(block.size() - 4) << 24 | block.at(block.size() - 4) << 24;

    if (blockType == BlockType::dataBlock)
    {

    }
}

bool B15Receiver::isDifferentFromPrevious(const uint8_t &nibble)
{
    if (nibble != previouslyReceivedNibble)
    {
        previouslyReceivedNibble = nibble;
        return true;
    }

    return false;
}

void B15Receiver::receive()
{
    const uint8_t input = drv.getRegister(&PORTA) >> 4;

    if (!isDifferentFromPrevious(input))
    {
        return;
    }

    decoder.nextNibble(input);
}