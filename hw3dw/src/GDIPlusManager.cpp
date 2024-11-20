#include "GDIPlusManager.hpp"
#include <gdiplus.h>

#include "Logging.hpp"

namespace Gdiplus
{
    using std::min;
    using std::max;
}

ULONG_PTR GdiPlusManager::token_ = {};

GdiPlusManager::GdiPlusManager()
{
    PLOGI << "Instantiate GDI Plus Manager";
}

GdiPlusManager::~GdiPlusManager()
{
    PLOGI << "Destroy GDI Plus Manager";
}

std::unique_ptr<GdiPlusManager> GdiPlusManager::initialize()
{
    PLOGI << "Initialize GDI Plus Manager";
    const Gdiplus::GdiplusStartupInput input;
    GdiplusStartup(&token_, &input, nullptr);
    return std::make_unique<GdiPlusManager>();
}

void GdiPlusManager::shutdown()
{
    PLOGI << "Shutdown GDI Plus Manager";
    Gdiplus::GdiplusShutdown(token_);
}