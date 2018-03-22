#include "thread_base.h"

using namespace std;

ThreadBase::ThreadBase()
    : m_started(false)
    , m_closing(false)
{
}

ThreadBase::~ThreadBase()
{
    StopThread();
}

void ThreadBase::StartThread()
{
    lock_guard<mutex> lock(m_critSec);

    if (m_started) 
        throw new runtime_error("Thread is already started.");

    m_thread = thread(&ThreadBase::ThreadProc, this);

    m_started = true;
    m_closing = false;

}

void ThreadBase::StopThread()
{
    return StopThread(true);
}

bool ThreadBase::CheckClosing() const
{
    return m_closing;
}

void ThreadBase::StopThread(bool wait)
{
    lock_guard<mutex> lock(m_critSec);

    if (m_started)
    {
        m_closing = true;

        if (wait)
            m_thread.join();
    }

    m_closing = false;
    m_started = false;
}

void ThreadBase::ThreadProc()
{
    try
    {
        OnThreadProc();
    }
    catch (exception)
    {
        // ignored
    }
}
