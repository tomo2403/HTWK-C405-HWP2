#pragma once

#include <cstdint>

struct InterthreadNotification
{
    enum class Type
    {
        START_SENDING_DATA,
	    CLOSE_CONNECTION,
        
        FOREIGN_PACKET_RESEND,
        FOREIGN_PACKET_RECEIVED,

        OWN_PACKET_RESEND,
        OWN_PACKET_RECEIVED
    };

    const Type type;
    const uint32_t referencedPacket_id;

    InterthreadNotification(const Type &type, const uint32_t &referencedPacket_id = 0);
};
