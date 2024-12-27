#pragma once

#include <cstdint>
#include <vector>
#include <thread>

#include "AtomicQueue.hpp"
#include "ICommunicationInterface.hpp"
#include "Receiver/Receiver.hpp"
#include "Sender/Sender.hpp"
#include "../header/Logger.hpp"

struct InterthreadNotification;

class ComManager final
{
	ICommunicationInterface *com;

	AtomicQueue<uint8_t> incomingQueue;
	AtomicQueue<uint8_t> outgoingQueue;
	AtomicQueue<InterthreadNotification> notifications;

	std::atomic<bool> running = true;
	std::vector<uint8_t> inputData{};
	std::vector<uint8_t> outputData{};

	Sender sender;
	Receiver receiver;

	void sendData();

	void receiveData();

public:
	explicit ComManager(ICommunicationInterface *com, const std::vector<uint8_t> &inputData);

	[[nodiscard]] std::vector<uint8_t> transfer2Way();
};
