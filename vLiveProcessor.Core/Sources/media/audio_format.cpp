#include "audio_format.h"

#include "common/strings.h"

AudioFormat::AudioFormat() 
    : AudioFormat(1, 8, 441000)
{
}

AudioFormat::AudioFormat(uint16_t channels, uint16_t bitsPerSample, uint16_t samplesPerSecond)
    : m_channels(channels)
    , m_bitsPerSample(bitsPerSample)
    , m_samplesPerSec(samplesPerSecond)
    , m_blockAlign(0)
    , m_avgBytesPerSec(0)
{
    m_blockAlign = m_channels * (m_bitsPerSample + 7) / 8;
    m_avgBytesPerSec = m_blockAlign * m_samplesPerSec;
}

AudioFormat::~AudioFormat()
{
}

std::ostream& operator<<(std::ostream& stream, const AudioFormat& format)
{
    return stream << StringFormat("Audio format (%d, %d, %d)", format.channels(), format.bitsPerSample(), format.samplesPerSec());
}
