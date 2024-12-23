#include "../header/InterthreadNotification.hpp"

InterthreadNotification::InterthreadNotification(const Type &type, const uint32_t &referencedPacket_id)
    : type(type), referencedPacket_id(referencedPacket_id)
{}