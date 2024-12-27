#include "ComInterface.hpp"

void ComInterface::openCom()
{
	std::lock_guard lock(mtx);
	Logger(DEBUG) << "Opening serial port...";
	const auto portName = SERIAL_PORT;
	serialPort = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);

	if (serialPort < 0)
	{
		Logger(ERROR) << "Error opening serial port!";
		exit(1);
	}

	termios tty{};
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

void ComInterface::closeCom()
{
	std::lock_guard lock(mtx);
	Logger(DEBUG) << "Closing interface port...";
	close(serialPort);
}

void ComInterface::writeByte(const uint8_t data)
{
	if (const ssize_t n = write(serialPort, &data, sizeof(data)); n < 0)
	{
		throw std::runtime_error("Error writing to serial port!");
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(WRITE_DELAY));
}

void ComInterface::readByte(uint8_t &data)
{
	if (const ssize_t m = read(serialPort, &data, sizeof(data)); m < 0)
	{
		throw std::runtime_error("Error reading from serial port!");
	}
}

bool ComInterface::isDataAvailable()
{
	int bytesAvailable;
	if (ioctl(serialPort, FIONREAD, &bytesAvailable) == -1)
	{
		throw std::runtime_error("Error checking available data on serial port!");
	}
	return bytesAvailable > 0;
}
