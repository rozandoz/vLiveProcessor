#pragma once

#include <mutex>

#include <winerror.h>

class ThreadBase
{
protected:
    ThreadBase();
    ~ThreadBase();

    HRESULT StartThread();
    HRESULT StopThread();

    bool CheckClosing() const;
    virtual void OnThreadProc() = 0;

private:
    HRESULT StopThread(bool wait);
    void ThreadProc();

    std::mutex       m_critSec;
    std::thread      m_thread;

    bool        m_started;
    bool        m_closing;
};
