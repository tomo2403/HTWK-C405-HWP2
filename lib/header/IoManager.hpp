#pragma once

#include <cstdint>
#include <vector>
#include <iostream>
#include <sys/fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <thread>

#include "Encoder.hpp"
#include "Decoder.hpp"
#include "CRC.hpp"
#include "ThreadSafeQueue.hpp"

/**
 * @class IoManager
 * @brief A class to manage input/output operations.
 */
class IoManager
{
protected:
    bool connected = false; /**< Indicates if the connection is established. */
    bool awaitingResponse = false; /**< Indicates if a response is awaited. */
    uint8_t escapeSequence; /**< The escape sequence byte. */
    uint8_t outboundChannel; /**< The outbound channel number. */
    int serialPort{}; /**< The file descriptor for the serial port. */
    CRC crc; /**< The CRC object for checksum calculations. */
    std::vector<PrePacket> outgoingPackets{}; /**< A vector to store packets. */
    std::vector<PrePacket> receivedPackets{};

	ThreadSafeQueue<StreamPacket> incomingQueue;
	ThreadSafeQueue<StreamPacket> outgoingQueue;

    /**
     * @brief Prepares packets from the given data.
     * @param data A reference to the data vector to be processed.
     */
    void preparePackets(std::vector<uint8_t> &data);

    /**
     * @brief Checks the response from the serial port.
     * @return True if the response is valid, false otherwise.
     * @param sp The packet to check.
     */
    [[ nodiscard ]] bool checkResponse(StreamPacket &sp);

    /**
     * @brief Sends a response packet over the serial port.
     * @param channel The channel number.
     * @param packetIndex The index of the packet.
     * @param success Indicates if the operation was successful.
     */
    void sendResponse(uint8_t channel, u_long packetIndex, bool success);

	void sendData(uint8_t channel, u_long packetIndex, std::vector<uint8_t> data);

	void processIncomingPacket(StreamPacket &sp);

	/**
	 * @brief Creates a stream packet from a pre-packet.
	 * @param p The pre-packet to convert.
	 * @param channel The channel number.
	 * @return The stream packet.
	 */
	StreamPacket createStreamPacket(PrePacket p, uint8_t channel);

	/**
	 * @brief Gets continuous input from the incoming port.
	 */
	virtual void getContinuesInput() = 0;

    /**
     * @brief Sends a packet over the serial port.
     * @param sp The packet to send.
     */
    virtual void sendPacket(const StreamPacket &sp) = 0;

    /**
     * @brief Receives a packet from the serial port.
     * @param sp The packet to receive.
     * @return The pre-packet received.
     */
    virtual PrePacket receivePacket(StreamPacket &sp) = 0;

public:
    /**
     * @brief Constructs an IoManager object.
     * @param escapeSequence The escape sequence byte.
     * @param crc The CRC object for checksum calculations.
     * @param outboundChannel The outbound channel number.
     */
    IoManager(uint8_t escapeSequence, CRC crc, uint8_t outboundChannel);

    /**
     * @brief Transfers data in a two-way communication.
     * @param input A reference to the input data vector.
     * @param output A reference to the output data vector.
     */
    void transfer2Way(std::vector<uint8_t> &input, std::vector<uint8_t> &output);

    /**
     * @brief Reads binary content from the standard input until EOF.
     * @return A vector containing the binary data read from the input.
     */
    static std::vector<uint8_t> getBinaryInput();

    /**
     * @brief Prints the contents of a vector to the standard output.
     * @param data A reference to the vector containing the data to be printed.
     */
    static void setBinaryOutput(const std::vector<uint8_t> &data);
};