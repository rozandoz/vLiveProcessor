#pragma once

#include "../common.h"
#include "../memory/memory_allocator.h"
#include "audio_format.h"


namespace common
{
    namespace media
    {
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
    }
}


