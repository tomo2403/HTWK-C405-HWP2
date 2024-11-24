#pragma once

#include <b15f/b15f.h>

class B15Communication
{
private:
	B15F &drv = B15F::getInstance();

	mutable std::mutex mtx;
	std::condition_variable cv;

public:
	void setDDRA(uint8_t value);

	uint8_t getDDRA();

	void setPORTA(uint8_t value);

	uint8_t getPINA();
};