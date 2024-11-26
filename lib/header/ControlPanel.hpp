#pragma once

#include <cstdint>

class ControlPanel
{
private:
	bool connected = false;
	bool everythingSent = false;
	bool everythingReceived = false;
	bool closeCmdReceived = false;

public:
	ControlPanel() = default;

	void processControlBlock(uint8_t &data);

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
