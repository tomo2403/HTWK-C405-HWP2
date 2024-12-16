#pragma once

#include <cstdint>

struct InterthreadNotification
{
    enum class Type
    {
        /** @brief The receiver requests a resend of the packet with id provided. */
        RESEND = 1 << 2, // Bit 2
        /** @brief The sender wants to establish a connection. */
        CONNECT = 1 << 3, // Bit 3
        /** @brief The receiver confirms that it has received the packet with id provided. */
        RECEIVED = 1 << 4, // Bit 4
    };

    const Type type;
    const uint16_t referencedPacket_id;

    InterthreadNotification(const Type &type, const uint16_t &referencedPacket_id);
};
