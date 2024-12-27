#include <b15f/b15f.h>
#include "../lib/header/AtomicQueue.hpp"
#include "../lib/header/InterthreadNotification.hpp"
#include "../lib/header/Sender/Sender.hpp"
#include "../lib/header/Receiver/Receiver.hpp"
#include "../lib/lib.hpp"

B15F &drv = B15F::getInstance();
uint8_t prevNibble{};

void com(AtomicQueue<uint8_t> *outgoingQueue, AtomicQueue<uint8_t> *incomingQueue, std::atomic<bool> *running)
{
    while (*running)
    {
        if (!outgoingQueue->empty())
        {
            drv.setRegister(&PORTA, outgoingQueue->wait_and_pop());
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        const uint8_t nibble = drv.getRegister(&PINA) >> 4;

        if (prevNibble != nibble)
        {
            prevNibble = nibble;
            incomingQueue->push(nibble);
            //std::cout << std::hex << (int) (drv.getRegister(&PINA) >> 4) << std::endl;
        }
    }
}

int main()
{
    AtomicQueue<uint8_t> incomingQueue;
    AtomicQueue<uint8_t> outgoingQueue;
    AtomicQueue<InterthreadNotification> notifications;

    std::atomic<bool> running = true;
    std::vector<uint8_t> inputData = ioManager::getBinaryInput();
    std::vector<uint8_t> outputData{};

    Sender sender(&outgoingQueue, &notifications, &running, inputData);
    Receiver receiver(&incomingQueue, &notifications, &running);

    drv.setRegister(&DDRA, 0x0F);

    std::thread send(&Sender::send, &sender);
    std::thread receive(&Receiver::receive, &receiver);

    std::thread comTh(com, &outgoingQueue, &incomingQueue, &running);

    comTh.join();
    send.join();
    receive.join();

    ioManager::setBinaryOutput(receiver.getOutputData());

    return 0;
}
