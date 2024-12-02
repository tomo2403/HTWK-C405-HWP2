#include <b15f/b15f.h>
#include "../lib/header/IDecoderObserver.hpp"
#include "../lib/header/Decoder.hpp"
#include "../lib/header/Encoder.hpp"

class B15Sender : IDecoderObserver
{
private:
    B15F& drv;
    uint32_t packetCounter;
    Decoder& decoder;
    Encoder& encoder;


public:
    B15Sender(B15F& drv, Decoder& decoder, Encoder& encoder);
    ~B15Sender();

    void beginBlockReceived(const BlockType &blockType) override;
    void endBlockReceived(const BlockType &blockType, std::vector<uint8_t> dataVector) override;

    void send();
};