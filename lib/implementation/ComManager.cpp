#include "../header/ComManager.hpp"

ComManager::ComManager(ICommunicationInterface *com, const std::vector<uint8_t> &inputData) : com(com), inputData(inputData),
	sender(&outgoingQueue, &notifications, &running, inputData, &cp), receiver(&incomingQueue, &notifications, &running, &cp)
{
}

void ComManager::sendData()
{
	while (running)
	{
		if (!outgoingQueue.empty())
		{
			com->writeByte(outgoingQueue.wait_and_pop());
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}


void ComManager::receiveData()
{
	while (running)
	{
		if (std::optional<uint8_t> byte = com->readByte())
		{
			incomingQueue.push(byte.value());
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}

void ComManager::updateDisplay()
{
	while (running)
	{
		cp.updateLog();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

std::vector<uint8_t> ComManager::transfer2Way()
{
	com->openCom();
	Logger(DEBUG) << "Starting transfer";

	std::thread send(&Sender::send, &sender);
	std::thread receive(&Receiver::receive, &receiver);
	std::thread sendThread(&ComManager::sendData, this);
	std::thread receiveThread(&ComManager::receiveData, this);
	std::thread cpThread(&ComManager::updateDisplay, this);

	send.join();
	receive.join();
	sendThread.join();
	receiveThread.join();
	cpThread.join();

	cp.updateLog();
	Logger(INFO) << "Transfer finished";
	com->closeCom();

	return receiver.getOutputData();
}
