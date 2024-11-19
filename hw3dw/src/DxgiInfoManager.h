#pragma once

#include <dxgidebug.h>
#include <string>
#include <vector>
#include <wrl.h>

class DxgiInfoManager
{
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;

	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;
	DxgiInfoManager(const DxgiInfoManager&&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&&) = delete;

	void set() noexcept;
	[[nodiscard]] std::vector<std::string> getMessages() const;
private:
	unsigned long long next_ = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue_;
};
