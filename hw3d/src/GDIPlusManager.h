#pragma once
#define GDI_PLUS_MANAGER_
#include "AtumWindows.h"
#include <memory>

class gdi_plus_manager
{
public:
    gdi_plus_manager();
    ~gdi_plus_manager();
    static std::unique_ptr<gdi_plus_manager> initialize();
    static void shutdown();

private:
    static ULONG_PTR token_;
};
