#include <b15f/b15f.h>
#include "../lib/header/IDecoderObserver.hpp"
#include "../lib/header/Decoder.hpp"

class B15Sender : IDecoderObserver
{
private:
    B15F& drv;
    uint32_t packetCounter;
    Decoder decoder;


public:
    B15Sender(B15F& drv);
    ~B15Sender();

    void beginBlockReceived(const BlockType &blockType) override;
    void endBlockReceived(const BlockType &blockType) override;

    void send();
};