#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../lib/Encoder.hpp"
#include "../lib/Decoder.hpp"
#include "../lib/ioManager.hpp"
#include <bitset>

std::atomic<bool> finishedDecoding(false);
std::mutex mtx;
std::condition_variable cv;

void setupSerialPort(int *serial_port)
{
	const char *portName = "/dev/ttyUSB0";
	*serial_port = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);

	if (*serial_port < 0)
	{
		std::cerr << "Error opening serial port!" << std::endl;
		exit(1);
	}

	struct termios tty{};
	memset(&tty, 0, sizeof(tty));

	if (tcgetattr(*serial_port, &tty) != 0)
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

	tcsetattr(*serial_port, TCSANOW, &tty);
}

void writeSerialPort(int serial_port, Encoder &enc)
{
	while (enc.hasData())
	{
		uint8_t message = enc.nextByte().value();
		ssize_t n = write(serial_port, &message, sizeof(message));
		if (n < 0)
		{
			throw std::runtime_error("Error writing to serial port!");
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void readSerialPort(int serial_port, Decoder &dec)
{
	while (!finishedDecoding)
	{
		uint8_t receivedData;
		ssize_t m = read(serial_port, &receivedData, sizeof(receivedData));
		if (m < 0)
		{
			throw std::runtime_error("Error reading from serial port!");
		}
		dec.nextNibble(receivedData);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main()
{
	int serial_port;
	setupSerialPort(&serial_port);

	bool finishedDecodingFlag = false;
	CommandObserver commandObserver(finishedDecodingFlag);

	std::vector<uint8_t> data = ioManager::getBinaryPipeContent();
	std::vector<uint8_t> receivedDataVector;
	Encoder enc(0x80, data);
	Decoder dec(0x80, receivedDataVector, &commandObserver);

	std::thread writer(writeSerialPort, serial_port, std::ref(enc));
	std::thread reader(readSerialPort, serial_port, std::ref(dec));

	writer.join();
	reader.join();

	close(serial_port);
	ioManager::printVector(receivedDataVector);

	return 0;
}