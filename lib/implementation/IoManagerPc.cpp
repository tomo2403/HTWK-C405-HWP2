#include "../header/IoManagerPc.hpp"

IoManagerPc::IoManagerPc(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel) : IoManager(escapeSequence, crc, outboundChannel)
{

}

void IoManagerPc::openSerialPort()
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

int IoManagerPc::closeSerialPort() const
{
	return close(serialPort);
}

ssize_t IoManagerPc::serialWrite(uint8_t data) const
{
	ssize_t n = write(serialPort, &data, sizeof(data));
	if (n < 0)
	{
		throw std::runtime_error("Error writing to serial port!");
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return n;
}

ssize_t IoManagerPc::serialRead() const
{
	uint8_t receivedData;
	ssize_t m = read(serialPort, &receivedData, sizeof(receivedData));
	if (m < 0)
	{
		throw std::runtime_error("Error reading from serial port!");
	}
	return receivedData;
}

bool IoManagerPc::isDataAvailable() const
{
	int bytesAvailable;
	if (ioctl(serialPort, FIONREAD, &bytesAvailable) == -1)
	{
		throw std::runtime_error("Error checking available data on serial port!");
	}
	return bytesAvailable > 0;
}

void IoManagerPc::sendPacket(const StreamPacket &sp)
{
	serialWrite(sp.channel);
	serialWrite(sp.dataLength);
	for (uint8_t byte : sp.data)
	{
		serialWrite(byte);
	}
}

void IoManagerPc::receivePacket(StreamPacket &sp)
{
	uint8_t channel = serialRead();
	uint8_t dataLength = serialRead();
	std::vector<uint8_t> data;
	for (int i = 0; i < dataLength; i++)
	{
		data.push_back(serialRead());
	}
	sp.channel = channel;
	sp.dataLength = dataLength;
	sp.data = data;
}

void IoManagerPc::processSerialInput()
{
	if (isDataAvailable())
	{

	}
}
