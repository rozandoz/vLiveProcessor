#include "memory_allocator.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

MemoryAllocator::MemoryAllocator(uint32_t bufferSize, uint32_t buffersCount)
{
    for (size_t i = 0; i < buffersCount; i++)
    {
        m_buffers.push(make_shared<DataBuffer>(bufferSize));
    }
}

MemoryAllocator::~MemoryAllocator()
{
}

void MemoryAllocator::ReturnBuffer(SharedDataBuffer sharedBuffer)
{
    lock_guard<mutex> lock(m_critSec);
    m_buffers.push(sharedBuffer);
}

shared_ptr<MemoryAllocator> MemoryAllocator::Create(uint32_t bufferSize, uint32_t buffersCount)
{
    return make_shared<MemoryAllocator>(bufferSize, buffersCount);
}

bool MemoryAllocator::TryGetBuffer(uint32_t timeout, shared_ptr<MemoryBuffer>& buffer)
{
    auto now = high_resolution_clock::now();
    auto start = now;

    while (duration_cast<milliseconds>(now - start) < milliseconds(timeout))
    {
        lock_guard<mutex> lock(m_critSec);
        {
            if (m_buffers.size() != 0)
            {
                buffer = make_shared<MemoryBuffer>(shared_from_this(), m_buffers.front());
                m_buffers.pop();
                return true;
            }
        }

        this_thread::sleep_for(1ms);
        now = high_resolution_clock::now();
    }

    return false;
}

MemoryAllocator::MemoryBuffer::MemoryBuffer(SharedAllocator owner, SharedDataBuffer sharedBuffer)
    : m_size(0)
    , m_owner(owner)
    , m_buffer(sharedBuffer)
{

}

MemoryAllocator::MemoryBuffer::~MemoryBuffer()
{
    if (m_owner)
    {
        m_owner->ReturnBuffer(m_buffer);
    }
}

char* MemoryAllocator::MemoryBuffer::data() const
{
    return m_buffer->data();
}

uint32_t MemoryAllocator::MemoryBuffer::max_size() const
{
    return m_buffer->size();
}

uint32_t MemoryAllocator::MemoryBuffer::size() const
{
    return m_size;
}

void MemoryAllocator::MemoryBuffer::set_size(uint32_t size)
{
    m_size = size;
}
