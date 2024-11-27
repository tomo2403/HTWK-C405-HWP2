#pragma once

#include <cstdint>
#include <vector>
#include "ThreadSafeQueue.hpp"

enum Flags : uint8_t {
	/** @brief The sender has delivered all data and is ready to close the connection. */
	TRANSFER_FINISHED = 1 << 0,  // Bit 0
	/** @brief The sender will close the connection and is not receiving any new packets. */
	CLOSE_CONNECTION  = 1 << 1,  // Bit 1
	/** @brief The receiver requests a resend of the packet with id provided. */
	RESEND            = 1 << 2,  // Bit 2
	/** @brief The sender wants to established a connection. */
	CONNECT           = 1 << 3,  // Bit 3
	/** @brief The receiver confirms that it has received the packet with id provided. */
	RECEIVED		  = 1 << 4,  // Bit 4
};

class ControlPanel
{
private:
	bool connected = false;
	bool everythingSent = false;
	bool everythingReceived = false;
	bool closeCmdReceived = false;

public:
	ThreadSafeQueue<uint32_t> resendRequests; /**< Packets that need to be resent. */
	ThreadSafeQueue<uint32_t> confirmedPackets; /**< Packets that were confirmed by the receiver. */

	ControlPanel() = default;

	/**
	 * @brief Processes a control block.
	 * @param flags The flags to process.
	 * @param packetId The packet id to process.
	 */
	void processControlBlock(uint8_t &flags, uint32_t &packetId);

	/**
	 * @brief Creates a control block.
	 * @param flags The flags to set.
	 * @param packetId The packet id to set.
	 * @return The created control block.
	 */
	[[nodiscard]] std::vector<uint8_t> createControlBlock(uint8_t flags, uint32_t packetId);

	/**
	 * @brief Checks if the connection is established.
	 * @return True if the connection is established, false otherwise.
	 */
	[[nodiscard]] bool isConnected() const;

	/**
	 * @brief Checks if everything was sent.
	 * @return True if everything was sent, false otherwise.
	 */
	[[nodiscard]] bool isEverythingSent() const;

	/**
	 * @brief Checks if everything was received.
	 * @return True if everything was received, false otherwise.
	 */
	[[nodiscard]] bool isEverythingReceived() const;

	/**
	 * @brief Checks if the close command was received.
	 * @return True if the close command was received, false otherwise.
	 */
	[[nodiscard]] bool isCloseCmdReceived() const;
};
