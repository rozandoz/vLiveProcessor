#pragma once

#include "common/common.h"
#include "common/memory/memory_allocator.h"
#include "audio_format.h"

class MediaBlock
{
public:
    MediaBlock(std::shared_ptr<Buffer> buffer, AudioFormat& audioFormat);
    ~MediaBlock();

    DECLARE_ACCESSOR(std::shared_ptr<Buffer>, buffer)
    DECLARE_ACCESSOR(AudioFormat, audioFormat)

private:
    std::shared_ptr<Buffer>     m_buffer;
    AudioFormat                 m_audioFormat;
};



