#include "media_block.h"

common::media::MediaBlock::MediaBlock(std::shared_ptr<Buffer> buffer, AudioFormat& audioFormat):
    m_buffer(buffer),
    m_audioFormat(audioFormat)
{
}

common::media::MediaBlock::~MediaBlock()
{
}