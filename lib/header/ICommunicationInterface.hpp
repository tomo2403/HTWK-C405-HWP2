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

#include "Logger.hpp"

#define SERIAL_PORT "/dev/cu.usbserial-140"

class ICommunicationInterface
{
public:
	[[nodiscard]] virtual bool isDataAvailable() = 0;

	virtual void writeByte(uint8_t data) = 0;

	virtual void readByte(uint8_t &data) = 0;

	virtual void openCom() = 0;

	virtual void closeCom() = 0;
};
