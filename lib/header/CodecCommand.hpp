#pragma once

/**
 * @brief This enum contains every pre-defined 4-bit sequence and its meaning.
 * Every command (except the escapeSequence) must be preceded by the escapeSequence.
 *
 * @details
 * Some commands have default and fallback versions. The fallback version is used
 * whenever the upcoming nibble after the command is the same as the nibble that represents the default command.
 * The fallback therefore prevents nibbles from being sent twice in succession.
 */
enum CodecCommand
{

    /**
     * @brief Only commands after the escapeSequence are recognized as such.
     *
     * @details
     * This escapeSequence is used to escape every other command.
     * It cannot occur in the transmitted data stream as data, as it would always be recognized as escapeSequence
     * and alter the behavior of the decoder.
     */
    escapeSequence = 0x00,

    /**
     * @brief A new data block begins.
     *
     * @details
     * This command is used to indicate the start of a new data block.
     */
    beginDataBlockDefault = 0x01,

    /**
     * @brief A new data block begins.
     *
     * @details
     * This command is used to indicate the start of a new data block.
     */
    beginDataBlockFallback = 0x02,

    /**
     * @brief A new control block begins.
     *
     * @details
     * This command is used to indicate the start of a new control block.
     */
    beginControlBlockDefault = 0x03,

    /**
     * @brief A new control block begins.
     *
     * @details
     * This command is used to indicate the start of a new control block.
     */
    beginControlBlockFallback = 0x04,

    /**
     * @brief The current data block ends.
     *
     * @details
     * This command is used to indicate the end of a data block.
     */
    endBlockDefault = 0x05,

    /**
     * @brief The current data block ends.
     *
     * @details
     * This command is used to indicate the end of a data block.
     */
    endBlockFallback = 0x0A,

    /**
     * @brief Inserts the previous nibble again.
     *
     * @details
     * If a nibble occurs back-to-back in the data stream, it is only sent once,
     * followed by this command.
     */
    insertPrevNibbleAgainDefault = 0x06,

    /**
     * @brief Inserts the previous nibble again.
     *
     * @details
     * If a nibble occurs back-to-back in the data stream, it is only sent once,
     * followed by this command.
     */
    insertPrevNibbleAgainFallback = 0x07,

    /**
     * @brief Inserts the escape sequence as data.
     *
     * @details
     * If the escape sequence occurs in the data stream, this command is placed after
     * it to inform the decoder to place the escape sequence back in the original data stream.
     */
    insertEscSeqAsDataDefault = 0x08,

    /**
     * @brief Inserts the escape sequence as data.
     *
     * @details
     * If the escape sequence occurs in the data stream, this command is placed after
     * it to inform the decoder to place the escape sequence back in the original data stream.
     */
    insertEscSeqAsDataFallback = 0x09,
};