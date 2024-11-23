#pragma once
#include <string>
#include <unordered_map>

#include "AtumWindows.hpp"

class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::string operator()(DWORD msg, LPARAM lParam, WPARAM wParam) const;
private:
	const std::unordered_map<DWORD, std::string> map_;
};
