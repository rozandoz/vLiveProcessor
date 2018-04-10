#include "media_block.h"

MediaBlock::MediaBlock(std::shared_ptr<Buffer> buffer, AudioFormat& audioFormat):
    m_buffer(buffer),
    m_audioFormat(audioFormat)
{
}

MediaBlock::~MediaBlock()
{
}