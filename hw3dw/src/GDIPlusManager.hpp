#pragma once

#include "AtumWindows.hpp"
#include <gdiplus.h>

class GdiPlusManager
{
public:
    GdiPlusManager();
    ~GdiPlusManager();

    GdiPlusManager(const GdiPlusManager&) = delete;
    GdiPlusManager& operator=(const GdiPlusManager&) = delete;
    GdiPlusManager(GdiPlusManager&&) = delete;
    GdiPlusManager& operator=(GdiPlusManager&&) = delete;

private:
    ULONG_PTR token_ = 0; // Instance-specific token
};
