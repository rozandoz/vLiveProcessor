#include "media_data.h"

MediaData::MediaData(std::shared_ptr<Buffer>& buffer)
    : m_buffer(buffer)
{
}

MediaData::~MediaData()
{
}

std::shared_ptr<Buffer> MediaData::buffer() const
{
    return m_buffer;
}
