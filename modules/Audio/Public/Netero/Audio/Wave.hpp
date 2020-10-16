/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file wave.hpp
 * @brief Contain struct declaration for wave file manipulation.
 */

#pragma pack(push, 1)

constexpr const uint32_t RIFF_Magic = 0x46464952;
constexpr const uint32_t WAVE_Magic = 0x45564157;
constexpr const uint32_t WAVE_Format_Chunck_Magic = 0x20746d66;
constexpr const uint32_t WAVE_Data_Chunk_Magic = 0x61746164;

/*
 * @brief Wave header chunk.
 */
struct WaveHeaderBlock {
    const uint32_t FileTypeBlockID = 0x46464952; /**< RIFF magic numbers. */
    uint32_t       FileSize = 0;                 /** The entire file size minus 8 bytes. */
    const uint32_t FileFormatID = 0x45564157;    /**< WAVE Magic numbers. */
};

/**
 * @brief Signal encode format identifiers.
 */
enum WaveAudioFormat {
    PCM = 0x0001,        /**< PCM signal encode identifier. */
    IEEE_FLOAT = 0x0003, /**< Float 32 signal encode identifier */
};

/**
 * @brief Wave format chunk.
 */
struct WaveFormatBlock {
    const uint32_t FormatBlockID = 0x20746d66; /**< Chunk magic ID. */
    const uint32_t BlockSize = 0x10;           /**< The size of the chunk in bytes. */
    uint16_t       AudioFormat = 0;            /**< Signal encode format identifier. */
    uint16_t       ChannelNbr = 0;             /**< Number of channels. */
    uint32_t       Frequency = 0;              /**< Sampling frequency. */
    uint32_t       BytePerSec = 0;             /**< Byte to read per second. */
    uint16_t       BytePerBlock = 0;           /**< Number of bytes per block. */
    uint16_t       BitsPerSample = 0;          /**< Number of bits per sample. */
};

/**
 * @brief Wave data chunk.
 */
struct WaveDataBlock {
    const uint32_t DataBlockID = 0x61746164; /**< Chunk magic ID. */
    uint32_t       DataSize = 0;             /**< Number of bytes minus the size of the header. */
};

/**
 * @brief Wave file header structure.
 */
struct WaveHeader {
    WaveHeaderBlock header; /**< Header chunk. */
    WaveFormatBlock format; /**< Format chunk. */
    WaveDataBlock   data;   /**< Data chunk. */
};

#pragma pack(pop)
