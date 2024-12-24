#include <utility>

#include "../header/ComManager.hpp"

ComManager::ComManager(ICommunicationInterface *com, const std::vector<uint8_t> &inputData) : com(com), inputData(inputData), sender(&dataQueue, &notifications, &running, inputData), receiver(&dataQueue, &notifications, &running)
{
}

void ComManager::sendData()
{
	while (running)
	{
		//TODO: implement sending data
		sender.send();
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}
}


void ComManager::receiveData()
{
	while (running)
	{
		//TODO: implement receiving data
		if (com->isDataAvailable())
		{
			uint8_t byte;
			com->readByte(byte);
		}
		receiver.receive();
		std::this_thread::sleep_for(std::chrono::nanoseconds(100));
	}
}



std::vector<uint8_t> ComManager::transfer2Way()
{
	com->openCom();
	std::thread sendThread(&ComManager::sendData, this);
	std::thread receiveThread(&ComManager::receiveData, this);

	sendThread.join();
	receiveThread.join();

	com->closeCom();
	return outputData;
}
