#include "IoManager.hpp"

IoManager::IoManager(uint8_t escapeSequence, CRC crc) : escapeSequence(escapeSequence), crc(crc)
{

}

void IoManager::send(std::vector<uint8_t> data)
{
	while (!connected) {}

	Encoder enc(escapeSequence, data);
	std::atomic<bool> timeoutFlag{false};
	std::thread timeoutThread(&IoManager::checkTimeout, this, std::ref(timeoutFlag), 5000); // 5 seconds timeout

	while (enc.hasData())
	{
		if (timeoutFlag.load())
		{
			setTimeoutOccurred();
			break;
		}

		uint8_t message = enc.nextByte().value();
		ssize_t n = write(serialPort, &message, sizeof(message));
		if (n < 0)
		{
			throw std::runtime_error("Error writing to serial port!");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	timeoutThread.join();
}

void IoManager::receive(std::vector<uint8_t> &data)
{
	Decoder dec(escapeSequence, data);
	while (dec.connectionIsOnline()) {}
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
