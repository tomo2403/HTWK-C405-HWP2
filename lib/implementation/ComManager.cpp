#include "../header/ComManager.hpp"
#include "../header/Logger.hpp"

ComManager::ComManager(ICommunicationInterface *com, const std::vector<uint8_t> &inputData) : com(com), inputData(inputData),
                                                                                              sender(&outgoingQueue, &notifications, &running, inputData), receiver(&incomingQueue, &notifications, &running)
{
}

void ComManager::sendData()
{
	while (running)
	{
		if (!outgoingQueue.empty())
		{
			//Logger(DEBUG) << "Sending byte";
			com->writeByte(outgoingQueue.wait_and_pop());
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}
}


void ComManager::receiveData()
{
	while (running)
	{
		if (com->isDataAvailable())
		{
			//Logger(DEBUG) << "Receiving byte";
			uint8_t byte;
			com->readByte(byte);
			incomingQueue.push(byte);
		}
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
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

	send.join();
	receive.join();
	sendThread.join();
	receiveThread.join();

	Logger(INFO) << "Transfer finished";
	com->closeCom();

	return receiver.getOutputData();
}
