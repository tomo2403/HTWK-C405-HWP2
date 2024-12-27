#pragma once

#include <cstdint>
#include <mutex>

class ICommunicationInterface
{
protected:
	mutable std::mutex mtx;

public:
	virtual ~ICommunicationInterface() = default;

	virtual void writeByte(uint8_t data) = 0;

	virtual std::optional<uint8_t> readByte() = 0;

	virtual void openCom() = 0;

	virtual void closeCom() = 0;
};
