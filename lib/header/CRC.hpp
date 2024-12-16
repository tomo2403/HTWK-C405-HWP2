#pragma once

#include <vector>
#include <cstdint>

/**
 * @brief Class for generating and validating a CRC value.
 *
 * @details
 * The class currently supports only the calculation of "ordinary" CRCs. Reflecting the input or output
 * is not (yet) possible. Currently, only 32-bit CRCs can be calculated.
 */
class CRC
{
public:
    /**
     * @brief Calculates a 32-bit CRC value over the given data.
     *
     * @details
     * The calculation is performed by mimicking a (hardware) shift register. The CRC is calculated in place
     * and without additional memory requirements. Each function call results in a new calculation.
     *
     * @param data Data over which the CRC value should be calculated.
     * @return uint32_t; (Only) 32-bit CRC value of the data.
     */
    [[nodiscard]] uint32_t calculateCRC(const std::vector<uint8_t> &data) const;

    void attachCRC(std::vector<uint8_t> &data) const;

    /**
     * @brief Validates a CRC value.
     * @param data Data for which a CRC value should be validated.
     * @param receivedCRC CRC value that is associated with the given data.
     * @see calculateCRC(myVector8) - Used to calculate the CRC value that will be compared with the given CRC value.
     * @return bool; TRUE if the CRC value matches the data; FALSE otherwise.
     */
    [[nodiscard]] bool validateCRC(const std::vector<uint8_t> &data, uint32_t receivedCRC) const;

private:
    uint32_t polynomial = 0x04c11db7;
    uint32_t initialValue = 0xffffffff;
};
