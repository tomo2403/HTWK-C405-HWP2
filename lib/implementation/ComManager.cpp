#include "../header/ComManager.hpp"

ComManager::ComManager(ICommunicationInterface *com, const std::vector<uint8_t> &inputData) : com(com), inputData(inputData),
	sender(&outgoingQueue, &notifications, &running, inputData), receiver(&incomingQueue, &notifications, &running)
{
}

void ComManager::sendData()
{
	std::thread send(&Sender::send, &sender);
	while (running)
	{
		if (!outgoingQueue.empty())
		{
			com->writeByte(outgoingQueue.wait_and_pop());
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}
	send.join();
}


void ComManager::receiveData()
{
	std::thread receive(&Receiver::receive, &receiver);
	while (running)
	{
		if (com->isDataAvailable())
		{
			uint8_t byte;
			com->readByte(byte);
			incomingQueue.push(byte);
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}
	receive.join();
}


std::vector<uint8_t> ComManager::transfer2Way()
{
	com->openCom();
	std::thread sendThread(&ComManager::sendData, this);
	std::thread receiveThread(&ComManager::receiveData, this);

	sendThread.join();
	receiveThread.join();

	com->closeCom();

	return receiver.getOutputData();
}
