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
    std::vector<uint8_t> data = ioManager::readFile("../../random.bin");
    // std::vector<uint8_t> data = ioManager::getBinaryInput();

    /*
    std::vector<uint8_t> data = {
    };
    */

    Sender sender(&dataQueue, &notifications, data);
    Receiver receiver (&dataQueue, &notifications, sender.getAtomicBoolean_pointer());

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
