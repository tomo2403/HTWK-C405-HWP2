#pragma once

#include "CRC.hpp"

enum Flag : uint8_t
{
	/** @brief The sender has delivered all data and is ready to close the connection. */
	TRANSFER_FINISHED = 1 << 0, // Bit 0
	/** @brief The sender will close the connection and is not receiving any new packets. */
	CLOSE_CONNECTION = 1 << 1, // Bit 1
	/** @brief The receiver requests a resend of the packet with id provided. */
	RESEND = 1 << 2, // Bit 2
	/** @brief The sender wants to establish a connection. */
	CONNECT = 1 << 3, // Bit 3
	/** @brief The receiver confirms that it has received the packet with id provided. */
	RECEIVED = 1 << 4, // Bit 4
};

class ControlPacketAssembler
{
	static void packetAssembly_addId(std::vector<uint8_t> &packet, const uint32_t &id);

	static void packetAssembly_addFlags(std::vector<uint8_t> &packet, const uint8_t &flags);

	static void packetAssembly_addCrc(std::vector<uint8_t> &packet);

public:
	ControlPacketAssembler() = delete;

	static std::vector<uint8_t> assemble(const uint8_t &flag, const uint32_t &packetId);
};
