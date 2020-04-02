/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

#pragma pack(push, 1)

struct WaveHeaderBlock {
    const uint32_t  FileTypeBlockID = 0x46464952;
    uint32_t        FileSize = 0;
    const uint32_t  FileFormatID = 0x45564157;
};

enum WaveAudioFormat {
    PCM = 0x0001,
    IEEE_FLOAT = 0x0003
};

struct WaveFormatBlock {
    const uint32_t  FormatBlockID = 0x20746d66;
    const uint32_t  BlockSize = 0x10;
    uint16_t        AudioFormat = 0;
    uint16_t        ChannelNbr = 0;
    uint32_t        Frequence = 0;
    uint32_t        BytePerSec = 0;
    uint16_t        BytePerBlock = 0;
    uint16_t        BitsPerSample = 0;
};

struct WaveDataBlock {
    const uint32_t  DataBlockID = 0x61746164;
    uint32_t        DataSize = 0;
};

struct WaveHeader {
    WaveHeaderBlock header;
    WaveFormatBlock format;
    WaveDataBlock   data;
};

#pragma pack(pop)
