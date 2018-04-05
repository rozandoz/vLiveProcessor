#pragma once

#include <cstdint>
#include <ostream>

#include "../common.h"

namespace common
{
    namespace media
    {
        class AudioFormat
        {
        public:
            AudioFormat(uint16_t channels, uint16_t bitsPerSample, uint16_t samplesPerSecond);
            ~AudioFormat();

            DECLARE_ACCESSOR(uint16_t, channels)
            DECLARE_ACCESSOR(uint16_t, bitsPerSample)
            DECLARE_ACCESSOR(uint32_t, samplesPerSec)
            DECLARE_ACCESSOR(uint16_t, blockAlign)
            DECLARE_ACCESSOR(uint32_t, avgBytesPerSec)

            friend std::ostream& operator<<(std::ostream& stream, const AudioFormat& format);

        private:
            uint16_t m_channels;
            uint16_t m_bitsPerSample;
            uint32_t m_samplesPerSec;

            uint16_t m_blockAlign;
            uint32_t m_avgBytesPerSec;
        };
    }
}
