#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../lib/header/Sender/Sender.hpp"
#include "../lib/header/Receiver/Receiver.hpp"
#include "../lib/lib.hpp"

int main() {
    AtomicQueue<uint8_t> dataQueue;
    AtomicQueue<InterthreadNotification> notifications;
    
    AtomicBoolean running;
    running = true;
    
    std::vector<uint8_t> data = ioManager::getBinaryInput();

    Sender sender(&dataQueue, &notifications, &running, data);
    Receiver receiver (&dataQueue, &notifications, &running);

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
