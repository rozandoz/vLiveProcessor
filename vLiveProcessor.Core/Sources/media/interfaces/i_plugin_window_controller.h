#pragma once

#include <cinttypes>

struct Rect
{
    int64_t    left;
    int64_t    top;
    int64_t    right;
    int64_t    bottom;
};

class IPluginWindowController
{
protected:
    virtual ~IPluginWindowController() {};

public:
    virtual void* GetWindow() = 0;
    virtual bool ResizeWindow(const Rect& rect) = 0;
    virtual Rect GetWindowRectangle() const = 0;
};
