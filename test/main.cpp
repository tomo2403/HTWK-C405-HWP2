#include <iostream>
#include <vector>
#include "../lib/header/DataPacketAssembler.hpp"
#include "../lib/header/DataPacketDisassembler.hpp"

int main() {
    std::vector<uint8_t> testData = {9, 3, 7, 1, 43, 4};
    
    DataPacketAssembler assembler(testData);
    
    uint16_t testPacketId = 0;

    std::vector<uint8_t> packet = assembler.getPacket(testPacketId);

    DataPacketDisassembler disassembler;

    bool result = disassembler.processPacket(packet, testPacketId);

    if (result)
    {
        std::cout << "Success" << std::endl;
    } else {
        std::cout << "Fail" << std::endl;
    }

    return 0;
}
