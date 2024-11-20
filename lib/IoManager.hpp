#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <sys/fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <thread>

#include "Encoder.hpp"
#include "Decoder.hpp"
#include "CRC.hpp"

/**
 * @class IoManager
 * @brief A class to manage input/output operations.
 */
class IoManager
{
private:
	bool connected = false;
	uint8_t escapeSequence;
	int serialPort{};
	CRC crc;
	std::atomic<bool> timeoutOccurred{false};

	void checkTimeout(std::atomic<bool> &timeoutFlag, int durationMs);

	void setTimeoutOccurred();

public:
	explicit IoManager(uint8_t escapeSequence, CRC crc);

	/**
	 * @brief Sets up the serial port with the specified settings.
	 * @param serial_port The serial port file descriptor.
	 */
	void openSerialPort();

	int closeSerialPort() const;

	void send(std::vector<uint8_t> data);

	void receive(std::vector<uint8_t> &data);

	/**
	 * @brief Reads binary content from the standard input until EOF.
	 * @return A vector containing the binary data read from the input.
	 */
	static std::vector<uint8_t> getBinaryPipeContent();

	/**
	 * @brief Prints the contents of a vector to the standard output.
	 * @param data A reference to the vector containing the data to be printed.
	 */
	static void printVector(const std::vector<uint8_t> &data);

	bool hasTimeoutOccurred() const { return timeoutOccurred.load(); }


};