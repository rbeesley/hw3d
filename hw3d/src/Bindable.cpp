#include "Bindable.h"

ID3D11DeviceContext* bindable::get_context(const graphics& graphics) noexcept
{
	return graphics.p_context_.Get();
}

ID3D11Device* bindable::get_device(const graphics& graphics) noexcept
{
	return graphics.p_device_.Get();
}

dxgi_info_manager& bindable::get_info_manager(graphics& graphics) noexcept(IS_DEBUG)
{
#if (IS_DEBUG)
	return graphics.info_manager_;
#else
	throw std::logic_error("Access denied: dxgi_info_manager is not accessible in release builds.");
#endif
}

