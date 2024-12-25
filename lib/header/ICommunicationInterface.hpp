#pragma once

#include <cstdint>
#include <mutex>

class ICommunicationInterface
{
protected:
	mutable std::mutex mtx;

public:
	virtual ~ICommunicationInterface() = default;

	[[nodiscard]] virtual bool isDataAvailable() = 0;

	virtual void writeByte(uint8_t data) = 0;

	virtual void readByte(uint8_t &data) = 0;

	virtual void openCom() = 0;

	virtual void closeCom() = 0;
};
