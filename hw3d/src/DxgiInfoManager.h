#pragma once

#include <dxgidebug.h>
#include <string>
#include <vector>
#include <wrl.h>

class dxgi_info_manager
{
public:
	dxgi_info_manager();
	~dxgi_info_manager() = default;
	dxgi_info_manager(const dxgi_info_manager&) = delete;
	dxgi_info_manager& operator=(const dxgi_info_manager&) = delete;
	dxgi_info_manager(const dxgi_info_manager&&) = delete;
	dxgi_info_manager& operator=(const dxgi_info_manager&&) = delete;
	void set() noexcept;
	[[nodiscard]] std::vector<std::string> get_messages() const;
private:
	unsigned long long next_ = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> p_dxgi_info_queue_;
};
