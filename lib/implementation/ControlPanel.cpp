#include "../header/ControlPanel.hpp"

#include <iomanip>

void ControlPanel::checkTransferStatus()
{
	if (finishedReceiving && finishedSending)
	{
		transferTime.stop();
	}
}

ControlPanel::ControlPanel()
{
	connectionTime.start();
	Logger(INFO) << "Connecting...";
}

ControlPanel::~ControlPanel()
{
	disconnectionTime.stop();
}

void ControlPanel::updateLog()
{
	if (connected)
	{
		Logger(INFO, true) << "Sent: " << sendPackets << (finishedSending ? " (done)" : "")
				<< " | Lost: " << lostPackets << " | Resend: " << resentPackets
				<< " || Received: " << receivedPackets << (finishedReceiving ? " (done)" : "")
				<< " | Faulty: " << receivedFaultyPackets
				<< " || " << std::fixed << std::setprecision(3) << transferTime.elapsed() << "s";
	}
	else if (!connected && connectionTime.running())
	{
		Logger(INFO, true) << "Connecting... " << std::fixed << std::setprecision(3) << connectionTime.elapsed() << "s";
	}
	else if (!connected && disconnectionTime.running())
	{
		Logger(INFO, true) << "Disconnecting... " << std::fixed << std::setprecision(3) << disconnectionTime.elapsed() << "s";
	}
}

void ControlPanel::incrementSendPackets()
{
	++sendPackets;
}

void ControlPanel::incrementReceivedPackets()
{
	++receivedPackets;
}

void ControlPanel::incrementLostPackets()
{
	++lostPackets;
}

void ControlPanel::incrementResentPackets()
{
	++resentPackets;
}

void ControlPanel::incrementReceivedFaultyPackets()
{
	++receivedFaultyPackets;
}

void ControlPanel::setConnected(const bool connected)
{
	if (this->connected == connected)
	{
		return;
	}
	this->connected = connected;

	if (connected)
	{
		connectionTime.stop();
		Logger(INFO) << "Sending...";
		transferTime.start();
		sendingTime.start();
	}
	else
	{
		disconnectionTime.start();
		Logger(INFO) << "Disconnecting... ";
	}
}

void ControlPanel::finishSending()
{
	finishedSending = true;
	sendingTime.stop();
	checkTransferStatus();
}

void ControlPanel::finishReceiving()
{
	finishedReceiving = true;
	checkTransferStatus();
}
