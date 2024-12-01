#include "../lib/lib.hpp"
#include "../lib/header/IDecoderObserver.hpp"
#include "../lib/header/Encoder.hpp"
#include "../lib/header/Decoder.hpp"
#include <iostream>

class Reciever : public IDecoderObserver
{
    void beginBlockReceived(const BlockType &blockType)
    {
        std::cout << "Start" << std::endl;
    }

    void endBlockReceived(const BlockType &blockType)
    {
        std::cout << "End" << std::endl;
    }
};

int main()
{
    std::vector<uint8_t> inputData_1 = {0x00, 0x00};
    std::vector<uint8_t> inputData_2 = {0x12, 0x34};
    Encoder encoder = Encoder();
    Decoder decoder = Decoder();

    encoder.inputDataBlock(inputData_1);
    decoder.addObserver(new Reciever());

    uint8_t i = 0;
    while(encoder.hasData())
    {
        if (i == 4) encoder.interruptWithControlBlock(inputData_2);
        uint8_t nN = encoder.nextNibble();
        std::cout << std::hex << static_cast<int>(nN) << std::endl;
        decoder.nextNibble(nN);
        i++;
    }

    for (auto i : decoder.getDataVector())
    {
        std::cout << std::hex << static_cast<int>(i) << std::endl;
    }

}