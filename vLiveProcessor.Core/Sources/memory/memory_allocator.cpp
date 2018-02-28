#include "memory_allocator.h"

#include <chrono>

using namespace std;
using namespace chrono;

MemoryAllocator::MemoryAllocator(uint32_t bufferSize, uint32_t buffersCount)
{
    for (size_t i = 0; i < buffersCount; i++)
    {
        m_buffers.push(make_shared<Buffer>(bufferSize));
    }
}

MemoryAllocator::~MemoryAllocator()
{
}

void MemoryAllocator::ReturnBuffer(SharedBuffer sharedBuffer)
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
    auto now = chrono::high_resolution_clock::now();
    auto start = now;

    while (duration_cast<chrono::milliseconds>(now - start) < chrono::milliseconds(timeout))
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

        Sleep(1);
        now = high_resolution_clock::now();
    }

    return false;
}

MemoryAllocator::MemoryBuffer::MemoryBuffer(SharedAllocator owner, SharedBuffer sharedBuffer)
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
