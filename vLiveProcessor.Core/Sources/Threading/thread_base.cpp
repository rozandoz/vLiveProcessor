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

HRESULT ThreadBase::StartThread()
{
    lock_guard<mutex> lock(m_critSec);

    try
    {
        if (m_started) return E_FAIL;

        m_thread = thread(&ThreadBase::ThreadProc, this);

        m_started = true;
        m_closing = false;

        return S_OK;
    }
    catch (exception)
    {
        return E_FAIL;
    }
}

HRESULT ThreadBase::StopThread()
{
    return StopThread(true);
}

bool ThreadBase::CheckClosing() const
{
    return m_closing;
}

HRESULT ThreadBase::StopThread(bool wait)
{
    lock_guard<mutex> lock(m_critSec);

    try
    {
        if (m_started)
        {
            m_closing = true;

            if (wait)
                m_thread.join();
        }

        m_closing = false;
        m_started = false;

        return S_OK;
    }
    catch (exception)
    {
        return E_FAIL;
    }
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
