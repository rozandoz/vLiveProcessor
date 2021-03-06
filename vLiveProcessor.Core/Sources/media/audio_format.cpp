#include "audio_format.h"

#include "common/strings.h"

using namespace std;
using namespace chrono;

AudioFormat& AudioFormat::Invalid()
{
    static AudioFormat invalidFormat(INVALID, 0, 0, 0);
    return invalidFormat;
}

duration<double> AudioFormat::GetDuration(const AudioFormat& format, size_t samples)
{
    return duration<double>(static_cast<double>(samples) / format.m_samplesPerSec);
}

seconds AudioFormat::GetDurationSec(const AudioFormat& format, size_t samples)
{
    return duration_cast<seconds>(GetDuration(format, samples));
}

milliseconds AudioFormat::GetDurationMs(const AudioFormat& format, size_t samples)
{
    return duration_cast<milliseconds>(GetDuration(format, samples));
}

nanoseconds AudioFormat::GetDurationNs(const AudioFormat& format, size_t samples)
{
    return duration_cast<nanoseconds>(GetDuration(format, samples));
}

AudioFormat::AudioFormat() 
    : AudioFormat(Invalid())
{
}

AudioFormat::AudioFormat(AudioType type, uint16_t channels, uint16_t bitsPerSample, uint32_t samplesPerSecond)
    : m_audioType(type)
    , m_channels(channels)
    , m_bitsPerSample(bitsPerSample)
    , m_samplesPerSec(samplesPerSecond)
    , m_blockAlign(0)
    , m_avgBytesPerSec(0)
{
    m_blockAlign = m_channels * ((m_bitsPerSample + 7) / 8);
    m_avgBytesPerSec = m_blockAlign * m_samplesPerSec;
}

AudioFormat::~AudioFormat()
{
}

std::ostream& operator<<(std::ostream& stream, const AudioFormat& format)
{
    return stream << StringFormat("Audio format (%d, %d, %d)", format.channels(), format.bitsPerSample(), format.samplesPerSec());
}

bool operator==(const AudioFormat& lhs, const AudioFormat& rhs)
{
    return lhs.m_audioType == rhs.m_audioType
        && lhs.m_channels == rhs.m_channels
        && lhs.m_bitsPerSample == rhs.m_bitsPerSample
        && lhs.m_samplesPerSec == rhs.m_samplesPerSec
        && lhs.m_blockAlign == rhs.m_blockAlign
        && lhs.m_avgBytesPerSec == rhs.m_avgBytesPerSec;
}

bool operator!=(const AudioFormat& lhs, const AudioFormat& rhs)
{
    return !(lhs == rhs);
}
