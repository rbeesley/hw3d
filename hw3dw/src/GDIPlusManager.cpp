#include "GDIPlusManager.h"
#include <gdiplus.h>

#include "Logging.h"

namespace Gdiplus
{
    using std::min;
    using std::max;
}

ULONG_PTR gdi_plus_manager::token_ = {};

gdi_plus_manager::gdi_plus_manager()
{
    PLOGI << "Instantiate GDI Plus Manager";
}

gdi_plus_manager::~gdi_plus_manager()
{
    PLOGI << "Destroy GDI Plus Manager";
}

std::unique_ptr<gdi_plus_manager> gdi_plus_manager::initialize()
{
    PLOGI << "Initialize GDI Plus Manager";
    const Gdiplus::GdiplusStartupInput input;
    GdiplusStartup(&token_, &input, nullptr);
    return std::make_unique<gdi_plus_manager>();
}

void gdi_plus_manager::shutdown()
{
    PLOGI << "Shutdown GDI Plus Manager";
    Gdiplus::GdiplusShutdown(token_);
}