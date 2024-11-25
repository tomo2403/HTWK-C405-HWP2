#include "Serial.hpp"
#include <cstring>

void Serial::openSerialPort()
{
	Logger(DEBUG) << "Opening serial port...";
	const char *portName = "/dev/cu.usbserial-140";
	serialPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);

	if (serialPort < 0)
	{
		Logger(ERROR) << "Error opening serial port!";
		exit(1);
	}

	struct termios tty{};
	memset(&tty, 0, sizeof(tty));

	if (tcgetattr(serialPort, &tty) != 0)
	{
		Logger(ERROR) << "Error getting port settings!";
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

	// Eingabe- und Ausgabe-Buffer leeren
	if (tcflush(serialPort, TCIOFLUSH) == -1)
	{
		Logger(ERROR) << "Failed to flush buffers.";
	}
	else
	{
		Logger(DEBUG) << "Serial buffers flushed successfully.";
	}

	Logger(INFO) << "Serial port open!";
}

int Serial::closeSerialPort() const
{
	Logger(DEBUG) << "Closing serial port...";
	return close(serialPort);
}

ssize_t Serial::serialWrite(uint8_t data) const
{
	ssize_t n = write(serialPort, &data, sizeof(data));
	if (n < 0)
	{
		throw std::runtime_error("Error writing to serial port!");
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return n;
}

ssize_t Serial::serialRead() const
{
	uint8_t receivedData;
	ssize_t m = read(serialPort, &receivedData, sizeof(receivedData));
	if (m < 0)
	{
		throw std::runtime_error("Error reading from serial port!");
	}
	return receivedData;
}

bool Serial::isDataAvailable() const
{
	int bytesAvailable;
	if (ioctl(serialPort, FIONREAD, &bytesAvailable) == -1)
	{
		throw std::runtime_error("Error checking available data on serial port!");
	}
	return bytesAvailable > 0;
}