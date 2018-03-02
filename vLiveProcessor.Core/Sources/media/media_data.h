#pragma once

#include "../memory/memory_allocator.h"

class MediaData
{
public:
    explicit MediaData(std::shared_ptr<Buffer>& buffer);
    ~MediaData();

    std::shared_ptr<Buffer> buffer() const;

private:
    std::shared_ptr<Buffer> m_buffer;
};
