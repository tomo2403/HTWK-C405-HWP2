#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../lib/header/Sender/Sender.hpp"

int main() {
    AtomicQueue<uint8_t> dataQueue;
    AtomicQueue<InterthreadNotification> notifications;
    std::vector<uint8_t> data = {0x78, 0x78, 0x78, 0x78};

    Sender sender(&dataQueue, &notifications, data);

    notifications.push(InterthreadNotification(InterthreadNotification::Type::START_SENDING_DATA, 0));

    std::thread senderThread([&sender]() {
        sender.send();
    });

    while (true) {
        uint8_t value = dataQueue.wait_and_pop();
        std::cout << "Gelesener Wert: " << static_cast<int>(value) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    senderThread.join();

    return 0;
}
