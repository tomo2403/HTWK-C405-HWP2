#include "../header/InterthreadNotification.hpp"

InterthreadNotification::InterthreadNotification(const Type &type, const uint16_t &referencedPacket_id)
    : type(type), referencedPacket_id(referencedPacket_id)
{}