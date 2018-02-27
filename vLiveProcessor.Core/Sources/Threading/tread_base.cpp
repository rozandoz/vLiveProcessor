#include "tread_base.h"

using namespace std;

tread_base::tread_base()
    : m_started(false)
    , m_closing(false)
{
}

tread_base::~tread_base()
{
    StopThread();
}

HRESULT tread_base::StartThread()
{
    lock_guard<mutex> lock(m_critSec);

    try
    {
        if (m_started) return E_FAIL;

        m_thread = thread(&tread_base::ThreadProc, this);

        m_started = true;
        m_closing = false;

        return S_OK;
    }
    catch (exception)
    {
        return E_FAIL;
    }
}

HRESULT tread_base::StopThread()
{
    return StopThread(true);
}

bool tread_base::CheckClosing() const
{
    return m_closing;
}

HRESULT tread_base::StopThread(bool wait)
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

void tread_base::ThreadProc()
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
