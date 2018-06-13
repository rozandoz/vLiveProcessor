#pragma once

#include <cstdint>
#include <ostream>
#include <chrono>

#include "common/common.h"

enum AudioType
{
    PCM,
    IEEE_FLOAT,
    INVALID = 0xFFFF,
};

class AudioFormat
{
public: //static
    static AudioFormat& Invalid();

    static std::chrono::duration<double>    GetDuration(const AudioFormat& format, size_t samples);
    static std::chrono::seconds             GetDurationSec(const AudioFormat& format, size_t samples);
    static std::chrono::milliseconds        GetDurationMs(const AudioFormat& format, size_t samples);
    static std::chrono::nanoseconds         GetDurationNs(const AudioFormat& format, size_t samples);

public:
    AudioFormat();
    AudioFormat(AudioType type, uint16_t channels, uint16_t bitsPerSample, uint32_t samplesPerSecond);
    ~AudioFormat();

    DECLARE_ACCESSOR(AudioType, audioType)
    DECLARE_ACCESSOR(uint16_t, channels)
    DECLARE_ACCESSOR(uint16_t, bitsPerSample)
    DECLARE_ACCESSOR(uint32_t, samplesPerSec)
    DECLARE_ACCESSOR(uint16_t, blockAlign)
    DECLARE_ACCESSOR(uint32_t, avgBytesPerSec)

    friend std::ostream& operator<<(std::ostream& stream, const AudioFormat& format);

    friend bool operator==(const AudioFormat& lhs, const AudioFormat& rhs);
    friend bool operator!=(const AudioFormat& lhs, const AudioFormat& rhs);

private:
    AudioType   m_audioType;

    uint16_t    m_channels;
    uint16_t    m_bitsPerSample;
    uint32_t    m_samplesPerSec;

    uint16_t    m_blockAlign;
    uint32_t    m_avgBytesPerSec;
};

#define INVALID_AUDIO_FORMAT AudioFormat::Invalid()