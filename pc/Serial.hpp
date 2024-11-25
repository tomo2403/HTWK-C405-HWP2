#pragma once

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <thread>

#include "../lib/header/Logger.hpp"

class Serial
{
public:
	int serialPort{}; /**< The file descriptor for the serial port. */

	/**
	 * @brief Writes a byte of data to the serial port.
	 * @param data The byte of data to write.
	 * @return The number of bytes written, or -1 on error.
	 */
	ssize_t serialWrite(uint8_t data) const;

	/**
	 * @brief Reads a byte of data from the serial port.
	 * @return The byte of data read, or -1 on error.
	 */
	ssize_t serialRead() const;

	/**
 	 * @brief Checks if data is available in the serial buffer.
	 * @return True if data is available, false otherwise.
	 */
	[[nodiscard]] bool isDataAvailable() const;

	/**
	 * @brief Sets up the serial port with the specified settings.
	 */
	void openSerialPort();

	/**
	 * @brief Closes the serial port.
	 * @return 0 if the serial port was closed successfully, -1 otherwise.
	 */
	int closeSerialPort() const;
};
