#pragma once
#define GDI_PLUS_MANAGER_
#include "AtumWindows.h"
#include <memory>

class GdiPlusManager
{
public:
    GdiPlusManager();
    ~GdiPlusManager();
    static std::unique_ptr<GdiPlusManager> initialize();
    static void shutdown();

private:
    static ULONG_PTR token_;
};
