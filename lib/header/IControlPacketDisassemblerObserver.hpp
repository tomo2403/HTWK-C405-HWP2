#pragma once

#include <vector>
#include <cstdint>

class IControlPacketDisassemblerObserver
{
protected:
	~IControlPacketDisassemblerObserver() = default;

public:
    virtual void on_received_received(const uint16_t &id) = 0;
    virtual void on_resend_received(const uint16_t &id) = 0;
    virtual void on_transferFinished_received() = 0;
    virtual void on_closeConnection_received() = 0;
    virtual void on_connect_received() = 0;
    virtual void on_packetCorrupt() = 0;
};