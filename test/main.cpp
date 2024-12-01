#include "../lib/lib.hpp"
#include "../lib/header/IDecoderObserver.hpp"
#include "../lib/header/Encoder.hpp"
#include "../lib/header/Decoder.hpp"
#include <iostream>

class Receiver : public IDecoderObserver
{
private:
    Decoder decoder = Decoder();

public:
    Receiver()
    {
        decoder.addObserver(this);
    }

    void beginBlockReceived(const BlockType &blockType)
    {
        std::cout << "Start" << std::endl;
    }

    void endBlockReceived(const BlockType &blockType, std::vector<uint8_t> dataVector)
    {
        std::cout << "Block-START" << std::endl;
        for (auto i : dataVector)
        {
            std::cout << std::hex << static_cast<int>(i) << std::endl;
        }
        std::cout << "Block-END" << std::endl;
    }

    void receive(const uint8_t &nibble)
    {
        decoder.nextNibble(nibble);
    }
};

int main()
{
    std::vector<uint8_t> inputData_1 = {0x00, 0x00, 0x00};
    std::vector<uint8_t> inputData_2 = {0x12, 0x34};
    Encoder encoder = Encoder();
    Receiver receiver = Receiver();

    encoder.inputDataBlock(inputData_1);

    uint8_t i = 0;
    while(encoder.hasData())
    {
        if (i == 4) encoder.interruptWithControlBlock(inputData_2);
        uint8_t nN = encoder.nextNibble();
        std::cout << std::hex << static_cast<int>(nN) << std::endl;
        receiver.receive(nN);
        i++;
    }

}