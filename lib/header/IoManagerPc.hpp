#pragma once

#include <sys/ioctl.h>
#include "IoManager.hpp"

class IoManagerPc : public IoManager
{
private:
    /**
     * @brief Writes a byte of data to the serial port.
     * @param data The byte of data to write.
     * @return The number of bytes written, or -1 on error.
     */
    ssize_t serialWrite(uint8_t data) const;

    /**
     * @brief Reads a byte of data from the serial port.
     * @return The byte of data read, or -1 on error.
     */
    ssize_t serialRead() const;

	void getContinuesInput() override;

    /**
     * @brief Sends a packet over the serial port.
     * @param sp The packet to send.
     */
    void sendPacket(const StreamPacket &sp) override;

    /**
     * @brief Receives a packet from the serial port.
     * @param sp The packet to receive.
     */
	void receivePacket(StreamPacket &sp) override;

    /**
     * @brief Processes the serial input.
     *
     */
    void processSerialInput(PrePacket &packet);

    /**
     * @brief Checks if data is available in the serial buffer.
     * @return True if data is available, false otherwise.
     */
    bool isDataAvailable() const;

public:
    /**
     * @brief Constructs an IoManagerPc object.
     * @param escapeSequence The escape sequence byte.
     * @param crc The CRC object for checksum calculations.
     * @param outboundChannel The outbound channel number.
     */
    IoManagerPc(CRC crc, uint8_t outboundChannel);

    /**
     * @brief Sets up the serial port with the specified settings.
     */
    void openSerialPort();

    /**
     * @brief Closes the serial port.
     * @return 0 if the serial port was closed successfully, -1 otherwise.
     */
    int closeSerialPort() const;
};