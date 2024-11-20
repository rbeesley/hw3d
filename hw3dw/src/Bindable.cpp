#include "Bindable.hpp"

ID3D11DeviceContext* Bindable::getContext(const Graphics& graphics) noexcept
{
	return graphics.deviceContext_.Get();
}

ID3D11Device* Bindable::getDevice(const Graphics& graphics) noexcept
{
	return graphics.device_.Get();
}

DxgiInfoManager& Bindable::getInfoManager(Graphics& graphics) noexcept(IS_DEBUG)
{
#if (IS_DEBUG)
	return graphics.infoManager_;
#else
	throw std::logic_error("Access denied: DxgiInfoManager is not accessible in release builds.");
#endif
}

