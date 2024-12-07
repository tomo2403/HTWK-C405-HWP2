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

#include "../lib/header/Logger.hpp"
#include "../lib/header/ICommunicationInterface.hpp"

#define SERIAL_PORT "/dev/cu.usbserial-140"

class ComInterface : public ICommunicationInterface
{
private:
	int serialPort{}; /**< The file descriptor for the serial port. */

public:
	/**
	 * @brief Writes a byte of data to the serial port.
	 * @param data The byte of data to write.
	 * @return The number of bytes written, or -1 on error.
	 */
	void writeByte(uint8_t data) override;

	/**
	 * @brief Reads a byte of data from the serial port.
	 * @return The byte of data read, or -1 on error.
	 */
	void readByte(uint8_t &data) override;

	/**
 	 * @brief Checks if data is available in the serial buffer.
	 * @return True if data is available, false otherwise.
	 */
	[[nodiscard]] bool isDataAvailable() override;

	/**
	 * @brief Sets up the serial port with the specified settings.
	 */
	void openCom() override;

	/**
	 * @brief Closes the serial port.
	 * @return 0 if the serial port was closed successfully, -1 otherwise.
	 */
	void closeCom() override;
};
