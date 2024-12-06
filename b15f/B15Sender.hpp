#include "b15global.hpp"
#ifndef DEBUG_MODE
    #include <b15f/b15f.h>
#endif
#include "../lib/lib.hpp"
#include "Timer.hpp"

class B15Sender : public IDecoderObserver
{
private:
    
    #ifdef DEBUG_MODE
        B15Fake& drv;
    #else
        B15F& drv;
    #endif

    uint16_t prevPacketID  = -1;
    Decoder& decoder;
    Encoder& encoder;
    CRC crcGenerator = CRC(0x00, 0x00);
    Timer timer = Timer();
    std::vector<uint8_t> rawDataToSend;
    bool connectionEstablished = false;
    const uint8_t packetSize = 64;

    std::vector<uint8_t> getRawDataById(const uint16_t &id);
    std::vector<uint8_t> getPackageById(const uint16_t &id);

public:

    #ifdef DEBUG_MODE
        B15Sender(B15Fake& drv, Decoder& decoder, Encoder& encoder, const std::vector<uint8_t> &rawDataToSend);
    #else
        B15Sender(B15F& drv, Decoder& decoder, Encoder& encoder, const std::vector<uint8_t> &rawDataToSend);
    #endif
    // ~B15Sender();

    void beginBlockReceived(const BlockType &blockType) override;
    void endBlockReceived(const BlockType &blockType, const std::vector<uint8_t> &dataVector) override;

    void send();
};