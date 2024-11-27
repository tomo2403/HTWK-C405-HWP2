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

    encoder.inputData(BlockType::dataBlock, inputData_1);
    decoder.addObserver(new Reciever());

    while(encoder.hasData())
    {
        uint8_t nN = encoder.nextNibble();
        std::cout << std::hex << static_cast<int>(nN) << std::endl;
        decoder.nextNibble(nN);
    }

    for (auto i : decoder.getDataVector())
    {
        std::cout << std::hex << static_cast<int>(i) << std::endl;
    }

    std::cout << "Neuer Vektor" << std::endl;
    encoder.inputData(BlockType::dataBlock, inputData_2);

    while(encoder.hasData())
    {
        uint8_t nN = encoder.nextNibble();
        std::cout << std::hex << static_cast<int>(nN) << std::endl;
        decoder.nextNibble(nN);
    }

    for (auto i : decoder.getDataVector())
    {
        std::cout << std::hex << static_cast<int>(i) << std::endl;
    }

}