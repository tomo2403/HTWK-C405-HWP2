#pragma once
#include <b15f/b15f.h>

class B15Communication
{
    private:
    B15F& drv = B15F::getInstance();

    mutable std::mutex mtx;
	std::condition_variable cv;

    public:
    void setDDRA(const uint8_t vlaue);
    uint8_t getDDRA();

    void setPORTA(const uint8_t vlaue);
    uint8_t getPINA();
};