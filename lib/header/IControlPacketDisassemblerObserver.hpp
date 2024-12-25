#pragma once

#include <cstdint>

class IControlPacketDisassemblerObserver
{
protected:
	~IControlPacketDisassemblerObserver() = default;

public:
    virtual void on_received_received(const uint32_t &id) = 0;
    virtual void on_resend_received(const uint32_t &id) = 0;
    virtual void on_transferFinished_received() = 0;
    virtual void on_closeConnection_received() = 0;
    virtual void on_connect_received() = 0;
    virtual void on_packetCorrupt() = 0;
};