#pragma once

class B15Fake
{
public:
    uint8_t getRegister(volatile uint8_t *adr)
    {
        return 0x00;
    };

    void setRegister(volatile uint8_t *adr, uint8_t val)
    {
        return;
    };
};