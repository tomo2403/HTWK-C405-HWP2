#include <b15f/b15f.h>
#include "../lib/lib.hpp"

class B15Sender : public IDecoderObserver
{
private:
    B15F& drv;
    uint32_t packetCounter;
    Decoder& decoder;
    Encoder& encoder;
    CRC crcGenerator = CRC(0x00, 0x00);


public:
    B15Sender(B15F& drv, Decoder& decoder, Encoder& encoder);
    ~B15Sender();

    void beginBlockReceived(const BlockType &blockType) override;
    void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) override;

    void send();
};