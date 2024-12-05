#include <b15f/b15f.h>
#include "../lib/lib.hpp"

class B15Receiver : IDecoderObserver
{
private:
    B15F& drv;
    uint16_t prevPacketID  = -1;
    Decoder& decoder;
    Encoder& encoder;
    ControlPanel controlPanel;
    uint8_t previouslyReceivedNibble;
    std::vector<uint8_t> receivedData;
    CRC crcGenerator = CRC(0x00, 0x00); // TODO: update CRC-Poly

    bool isDifferentFromPrevious(const uint8_t &nibble);

public:

    void beginBlockReceived(const BlockType &blockType) override;
    void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) override;

    void receive();

    B15Receiver(B15F& drv, Decoder& decoder, Encoder &encoder);
    // ~B15Receiver();
};
