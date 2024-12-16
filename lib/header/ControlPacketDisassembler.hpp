#pragma once

#include "IControlPacketDisassemblerObserver.hpp"

class ControlPacketDisassembler
{
private:

    std::vector<IControlPacketDisassemblerObserver *> observers;

public:

    void addObserver(IControlPacketDisassemblerObserver *observer);

	void removeObserver(IControlPacketDisassemblerObserver *observer);
};