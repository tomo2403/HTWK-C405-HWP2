#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../lib/header/Sender/Sender.hpp"
#include "../lib/header/Receiver/Receiver.hpp"

int main() {
    AtomicQueue<uint8_t> dataQueue;
    AtomicQueue<InterthreadNotification> notifications;
    std::vector<uint8_t> data = {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78};

    Sender sender(&dataQueue, &notifications, data);
    Receiver receiver (&dataQueue, &notifications);

    std::thread senderThread([&sender]() {
        sender.send();
    });

    std::thread receiverThread([&receiver]() {
        receiver.receive();
    });

    senderThread.join();
    receiverThread.join();

    return 0;
}
