
#include "GdiPlusManager.hpp"
#include "Logging.hpp"

namespace Gdiplus
{
    using std::min;
    using std::max;
}

GdiPlusManager::GdiPlusManager()
{
    PLOGI << "Instantiate GDI Plus Manager";

    Gdiplus::GdiplusStartupInput input;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&token_, &input, nullptr);

    if (status != Gdiplus::Ok)
    {
        PLOGE << "Failed to initialize GDI+";
        throw std::runtime_error("GDI+ initialization failed");
    }

    PLOGI << "GDI Plus Manager Initialized";
}

GdiPlusManager::~GdiPlusManager()
{
    PLOGI << "Destroy GDI Plus Manager";

    if (token_ != 0)
    {
        Gdiplus::GdiplusShutdown(token_);
        token_ = 0;
        PLOGI << "GDI Plus Manager Shutdown Complete";
    }
}
