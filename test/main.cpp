#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../lib/header/Sender/Sender.hpp"

int main() {
    AtomicQueue<uint8_t> dataQueue;
    AtomicQueue<InterthreadNotification> notifications;
    std::vector<uint8_t> data = {0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78};

    Sender sender(&dataQueue, &notifications, data);

    notifications.push(InterthreadNotification(InterthreadNotification::Type::START_SENDING_DATA, 0));

    std::thread senderThread([&sender]() {
        sender.send();
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    notifications.push(InterthreadNotification(InterthreadNotification::Type::OWN_PACKET_RECEIVED, 0));
    // notifications.push(InterthreadNotification(InterthreadNotification::Type::CLOSE_CONNECTION, 0));

    while (true) {
        uint8_t value = dataQueue.wait_and_pop();
        std::cout << "Gelesener Wert: " << static_cast<int>(value) << std::endl;

    }

    senderThread.join();

    return 0;
}
