#pragma once

#include <vector>
#include <cstdint>

/**
 * @brief A read-only FIFO queue for data on a nibble basis (stored in a compressed format within bytes).
 *
 * This class is designed to take in data during construction, which can only be read
 * and not modified afterward. The data is stored in bytes but is accessed in nibbles.
 */
class DataStorage
{
private:
	/**
	 * @brief Storage container for the data.
	 *
	 * This vector holds the data in bytes. The data is read out in nibbles.
	 */
	std::vector<uint8_t> dataVector_byte;

	/**
	 * @brief Index of the first byte that has not been fully read.
	 *
	 * This index points to the first byte in 'dataVector_byte' that contains
	 * nibbles that have not yet been read. It helps track the reading progress.
	 */
	uint32_t dataVectorOffset;

	/**
	 * @brief Indicates whether the next nibble to be read is the high nibble of the current byte.
	 *
	 * This boolean flag indicates if the next nibble to be read is the high nibble
	 * (4 most significant bits) or the low nibble (4 least significant bits) of the
	 * byte at the current 'dataVectorOffset'. If 'nextNibbleIsHighNibble' is true,
	 * the next nibble to be read is the high nibble.
	 */
	bool nextNibbleIsHighNibble;

public:
	/**
	 * @brief Constructs a DataStorage object with the given byte data.
	 * @param dataVector_byte A non-empty vector of bytes to be stored in the DataStorage.
	 * @throws 'std::invalid_argument' if 'dataVector_byte' is empty.
	 */
	explicit DataStorage(const std::vector<uint8_t> &dataVector_byte);

	/**
	 * @brief Peeks at the next nibble without removing it from the storage.
	 *
	 * This method allows you to view the next nibble that will be read without modifying
	 * the state of the storage.
	 *
	 * @return The next nibble (4 bits) to be read from the storage.
	 * @throws 'std::out_of_range' if the storage is empty and has no more nibbles to be read.
	 */
	[[nodiscard]] uint8_t peek_nibble() const;

	/**
	 * @brief Pops the next nibble from the storage.
	 *
	 * This function removes the next nibble from the storage and returns it. Internally
	 * it calls 'peek_nibble()' to obtain the next nibble and then removes it from the storage.
	 *
	 * @return The next nibble (4 bits) that was read from the storage.
	 * @throws 'std::out_of_range' if the storage is empty and has no more nibbles to be read.
	 * @see this->peek_nibble() if you do not wish to remove the nibble after accessing it.
	 */
	uint8_t pop_nibble();

	/**
	 * @brief Checks if there are any nibbles left to read.
	 * @return true, if there are no more nibbles to read; false, otherwise.
	 */
	[[nodiscard]] bool empty() const;
};
