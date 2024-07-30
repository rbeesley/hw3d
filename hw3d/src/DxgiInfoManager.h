#pragma once
#include "AtumWindows.h"

#include <string>
#include <vector>

class dxgi_info_manager
{
public:
	dxgi_info_manager();
	~dxgi_info_manager();
	dxgi_info_manager(const dxgi_info_manager&) = delete;
	dxgi_info_manager& operator=(const dxgi_info_manager&) = delete;
	dxgi_info_manager(const dxgi_info_manager&&) = delete;
	dxgi_info_manager& operator=(const dxgi_info_manager&&) = delete;
	void set() noexcept;
	[[nodiscard]] std::vector<std::string> get_messages() const;
private:
	unsigned long long next_ = 0u;
	struct IDXGIInfoQueue* dxgi_info_queue_ = nullptr;
};
