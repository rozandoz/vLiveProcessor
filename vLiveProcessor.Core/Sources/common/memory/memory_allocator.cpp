#include "memory_allocator.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

MemoryAllocator::MemoryAllocator(size_t bufferSize, size_t buffersCount)
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

shared_ptr<MemoryAllocator> MemoryAllocator::Create(size_t bufferSize, size_t buffersCount)
{
    return make_shared<MemoryAllocator>(bufferSize, buffersCount);
}

bool MemoryAllocator::TryGetBuffer(milliseconds timeout, shared_ptr<MemoryBuffer>& buffer)
{
    auto start = high_resolution_clock::now();

    do
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
        
    } while (high_resolution_clock::now() - start < timeout);

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

size_t MemoryAllocator::MemoryBuffer::max_size() const
{
    return m_buffer->size();
}

size_t MemoryAllocator::MemoryBuffer::size() const
{
    return m_size;
}

void MemoryAllocator::MemoryBuffer::set_size(size_t size)
{
    m_size = size;
}
