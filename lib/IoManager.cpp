#include "IoManager.hpp"

IoManager::IoManager(uint8_t escapeSequence, CRC crc) : escapeSequence(escapeSequence), crc(crc)
{

}

void IoManager::send()
{
	while (!connected)
	{ }

	u_long index = 0;

	for (int i = 0; i < packets.size(); i++) {
		sendPacket(packets[i]);

		if (!checkResponse()) {
			// resend packet
			i--;
		}
	}

	// TODO: send close connection command
}

void IoManager::receive(std::vector<uint8_t> &data)
{
	Decoder dec(escapeSequence, data);
	while (dec.connectionIsOnline())
	{ }
	connected = true;

	std::atomic<bool> timeoutFlag{false};
	std::thread timeoutThread(&IoManager::checkTimeout, this, std::ref(timeoutFlag), 5000); // 5 seconds timeout

	while (dec.hasData())
	{
		if (timeoutFlag.load())
		{
			setTimeoutOccurred();
			break;
		}

		uint8_t receivedData;
		ssize_t m = read(serialPort, &receivedData, sizeof(receivedData));
		if (m < 0)
		{
			throw std::runtime_error("Error reading from serial port!");
		}
		dec.nextNibble(receivedData);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	timeoutThread.join();
}

std::vector<uint8_t> IoManager::getBinaryPipeContent()
{
	std::vector<uint8_t> data;
	char ch;
	while (std::cin.get(ch))
	{
		data.push_back(static_cast<uint8_t>(ch));
	}
	if (std::cin.eof())
	{
		std::cin.clear();
	}
	return data;
}

void IoManager::printVector(const std::vector<uint8_t> &data)
{
	std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
}

void IoManager::openSerialPort()
{
	const char *portName = "/dev/ttyUSB0";
	serialPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);

	if (serialPort < 0)
	{
		std::cerr << "Error opening serial port!" << std::endl;
		exit(1);
	}

	struct termios tty{};
	memset(&tty, 0, sizeof(tty));

	if (tcgetattr(serialPort, &tty) != 0)
	{
		std::cerr << "Error getting port settings!" << std::endl;
		exit(1);
	}

	cfsetispeed(&tty, B57600);
	cfsetospeed(&tty, B57600);
	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;
	tty.c_cflag |= CREAD | CLOCAL;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_oflag &= ~OPOST;
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	tcsetattr(serialPort, TCSANOW, &tty);
}

int IoManager::closeSerialPort() const
{
	return close(serialPort);
}

void IoManager::checkTimeout(std::atomic<bool> &timeoutFlag, int durationMs)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
	timeoutFlag.store(true);
}

void IoManager::setTimeoutOccurred()
{
	timeoutOccurred.store(true);
	std::cerr << "Timeout occurred during receive operation!" << std::endl;
}

void IoManager::preparePackets(std::vector<uint8_t> &data)
{
	u_long dataIndex = 0;
	u_long packetIndex = 0;

	while (dataIndex < data.size())
	{
		PrePacket p{};
		p.index = packetIndex++;
		for (int i = 0; i < 64 && dataIndex < data.size(); i++)
		{
			p.data.push_back(data[dataIndex++]);
		}
		std::vector<uint8_t> dataVector = {p.index};
		dataVector.insert(dataVector.end(), p.data.begin(), p.data.end());
		p.crc = crc.calculateCRC(dataVector);

		packets.push_back(p);
	}
}

void IoManager::sendPacket(const StreamPacket &sp)
{
	std::vector<uint8_t> packetData = {sp.packet.index};
	packetData.insert(packetData.end(), sp.packet.data.begin(), sp.packet.data.end());
	packetData.push_back(sp.packet.crc);
	Encoder enc(escapeSequence, packetData);

	while (enc.hasData())
	{
		uint8_t message = enc.nextByte().value();
		ssize_t n = write(serialPort, &message, sizeof(message));
		if (n < 0)
		{
			throw std::runtime_error("Error writing to serial port!");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool IoManager::checkResponse()
{
	std::vector<uint8_t> data;
	Decoder dec(escapeSequence, data);

	std::atomic<bool> timeoutFlag{false};
	std::thread timeoutThread(&IoManager::checkTimeout, this, std::ref(timeoutFlag), 5000); // 5 seconds timeout

	uint8_t receivedData;
	ssize_t m = read(serialPort, &receivedData, sizeof(receivedData));
	if (m < 0)
	{
		throw std::runtime_error("Error reading from serial port!");
	}
	dec.nextNibble(receivedData);

	timeoutThread.join();

	if (timeoutFlag.load())
	{
		return false;
	}

	// TODO: check if response is valid
	return true;
}

void IoManager::sendResponse(uint8_t channel, u_long packetIndex, bool success)
{
	PrePacket p{};
	p.data.push_back(packetIndex);
	p.data.push_back(success ? 0x01 : 0x00);

	StreamPacket sp{};
	sp.channel = channel;
	sp.type = PacketType::Response;
	sp.packet = p;
}

