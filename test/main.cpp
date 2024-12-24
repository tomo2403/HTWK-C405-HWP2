#include <vector>
#include <thread>
#include "../lib/header/Sender/Sender.hpp"
#include "../lib/header/Receiver/Receiver.hpp"
#include "../lib/lib.hpp"

int main()
{
    AtomicQueue<uint8_t> dataQueue;
    AtomicQueue<InterthreadNotification> notifications;

    std::atomic running = true;

    //std::vector<uint8_t> data = ioManager::getBinaryInput();
    std::vector<uint8_t> data = ioManager::readFile("../../random.bin");

    Sender sender(&dataQueue, &notifications, &running, data);
    Receiver receiver(&dataQueue, &notifications, &running);

    std::thread senderThread([&sender, &running]()
    {
        while (running)
        {
            sender.send();
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
    });

    std::thread receiverThread([&receiver, &running]()
    {
        receiver.receive();
        while (running)
        {
            receiver.receive();
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
    });

    senderThread.join();
    receiverThread.join();

    return 0;
}
