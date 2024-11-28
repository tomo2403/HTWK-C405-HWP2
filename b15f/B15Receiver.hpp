#include <b15f/b15f.h>
#include "../lib/header/IDecoderObserver.hpp"
#include "../lib/header/Decoder.hpp"

class B15Receiver : IDecoderObserver
{
private:
    B15F& drv;
    uint16_t packetCounter;
    Decoder decoder;
    uint8_t previouslyReceivedNibble;
    std::vector<uint8_t> receivedData;

    bool isDifferentFromPrevious(const uint8_t &nibble);

public:

    void beginBlockReceived(const BlockType &blockType) override;
    void endBlockReceived(const BlockType &blockType) override;

    void receive();

    B15Receiver(B15F& drv);
    ~B15Receiver();
};
