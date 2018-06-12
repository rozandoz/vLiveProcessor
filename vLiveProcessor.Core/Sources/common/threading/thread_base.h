#pragma once

#include <mutex>

class ThreadBase
{
protected:
    ThreadBase();
    ~ThreadBase();

    void StartThread();
    void StopThread();

    bool CheckClosing() const;
    bool CheckStarted() const;
    virtual void OnThreadProc() = 0;

    void* GetNativeHandle();

private:
    void StopThread(bool wait);
    void ThreadProc();

    std::mutex      m_critSec;
    std::thread     m_thread;

    bool            m_started;
    bool            m_closing;
};
