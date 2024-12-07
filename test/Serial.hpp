#pragma once

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <b15f/b15f.h>

#include "../lib/header/Logger.hpp"

class Serial
{
public:
    B15F& drv = B15F::getInstance();
	uint8_t previouslyReceivedNibble{};
    std::mutex drvMutex;

	/**
	 * @brief Writes a byte of data to the serial port.
	 * @param data The byte of data to write.
	 * @return The number of bytes written, or -1 on error.
	 */
	ssize_t writeByte(uint8_t data);

	/**
	 * @brief Reads a byte of data from the serial port.
	 * @return The byte of data read, or -1 on error.
	 */
	ssize_t readByte(uint8_t &data);

	/**
 	 * @brief Checks if data is available in the serial buffer.
	 * @return True if data is available, false otherwise.
	 */
	[[nodiscard]] bool isDataAvailable();

	/**
	 * @brief Sets up the serial port with the specified settings.
	 */
	void openPort();

	/**
	 * @brief Closes the serial port.
	 * @return 0 if the serial port was closed successfully, -1 otherwise.
	 */
	int closePort() const;
};
