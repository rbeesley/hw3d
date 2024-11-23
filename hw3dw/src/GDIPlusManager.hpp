#pragma once

#include "AtumWindows.hpp"
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
