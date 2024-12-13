#pragma once

#include <cstdint>

class ICommunicationInterface
{
public:
	virtual ~ICommunicationInterface() = default;

	[[nodiscard]] virtual bool isDataAvailable() = 0;

	virtual void writeByte(uint8_t data) = 0;

	virtual void readByte(uint8_t &data) = 0;

	virtual void openCom() = 0;

	virtual void closeCom() = 0;
};
