#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <mutex>

class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator>
{
public:
    class MemoryBuffer;
    friend class std::_Ref_count_obj<MemoryAllocator>;

private:
    typedef std::vector<char>                    Buffer;
    typedef std::shared_ptr<Buffer>              SharedBuffer;
    typedef std::shared_ptr<MemoryAllocator>     SharedAllocator;

    MemoryAllocator(uint32_t bufferSize, uint32_t buffersCount);
    ~MemoryAllocator();

    void ReturnBuffer(SharedBuffer sharedBuffer);

public:
    static SharedAllocator Create(uint32_t bufferSize, uint32_t buffersCount);
    bool TryGetBuffer(uint32_t timeout, std::shared_ptr<MemoryBuffer>& buffer);

private:
    std::mutex                          m_critSec;
    std::queue<SharedBuffer>            m_buffers;

public:
    class MemoryBuffer
    {
        friend class std::_Ref_count_obj<MemoryBuffer>;

        MemoryBuffer(SharedAllocator owner, SharedBuffer sharedBuffer);
        ~MemoryBuffer();

    public:
        char* data() const;
        uint32_t max_size() const;
        uint32_t size() const;
        void set_size(uint32_t size);

    private:
        uint32_t                m_size;

        SharedAllocator         m_owner;
        SharedBuffer            m_buffer;
    };
};

typedef MemoryAllocator::MemoryBuffer   Buffer;

