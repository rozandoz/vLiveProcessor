#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>

class MemoryAllocator : public std::enable_shared_from_this<MemoryAllocator>
{
    friend class std::_Ref_count_obj<MemoryAllocator>;

public:
    class MemoryBuffer;

private:
    typedef std::vector<char>                    DataBuffer;
    typedef std::shared_ptr<DataBuffer>          SharedDataBuffer;
    typedef std::shared_ptr<MemoryAllocator>     SharedAllocator;

    MemoryAllocator(size_t bufferSize, size_t buffersCount);
    ~MemoryAllocator();

    void ReturnBuffer(SharedDataBuffer sharedBuffer);

public:
    static SharedAllocator Create(size_t bufferSize, size_t buffersCount);
    bool TryGetBuffer(std::chrono::nanoseconds timeout, std::shared_ptr<MemoryBuffer>& buffer);

private:
    std::mutex                          m_critSec;
    std::queue<SharedDataBuffer>        m_buffers;

public:
    class MemoryBuffer
    {
        friend class std::_Ref_count_obj<MemoryBuffer>;

        MemoryBuffer(SharedAllocator owner, SharedDataBuffer sharedBuffer);
        ~MemoryBuffer();

    public:
        char* data() const;
        size_t max_size() const;
        size_t size() const;
        void set_size(size_t size);

    private:
        size_t                  m_size;

        SharedAllocator         m_owner;
        SharedDataBuffer        m_buffer;
    };
};

typedef MemoryAllocator::MemoryBuffer   Buffer;

