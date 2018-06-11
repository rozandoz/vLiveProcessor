#pragma once

#include "media/interfaces/i_plugin_window_controller.h"

#include "Windows.h"

class TestWindowController : public IPluginWindowController
{
public:
    TestWindowController()
        : m_window(nullptr) {
    }

    virtual ~TestWindowController()
    {
        CloseWindow(m_window);
    }

public:
    void* GetWindow() override
    {
        WNDCLASSEX wcex{ sizeof(wcex) };
        wcex.lpfnWndProc = DefWindowProc;
        wcex.hInstance = GetModuleHandle(0);
        wcex.lpszClassName = L"Minimal VST host - Guest VST Window Frame";
        RegisterClassEx(&wcex);

        const auto style = WS_CAPTION | WS_THICKFRAME | WS_OVERLAPPEDWINDOW;
        m_window = CreateWindow(wcex.lpszClassName, L"Window", style, 0, 0, 0, 0, GetConsoleWindow(), 0, 0, 0);

        ShowWindow(m_window, SW_SHOW);
        return m_window;
    }

    bool ResizeWindow(const Rect& rect) override
    {
        RECT clientRc = { rect.left, rect.top, rect.right, rect.bottom };

        auto rc = clientRc;
        const auto style = GetWindowLongPtr(m_window, GWL_STYLE);
        const auto exStyle = GetWindowLongPtr(m_window, GWL_EXSTYLE);
        const BOOL fMenu = GetMenu(m_window) != nullptr;
        AdjustWindowRectEx(&rc, style, fMenu, exStyle);
        return MoveWindow(m_window, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }

    Rect GetWindowRectangle() const override
    {
        RECT rect;
        GetWindowRect(m_window, &rect);
        return Rect { rect.left, rect.top, rect.right, rect.bottom };
    }

private:
    HWND m_window;
};
