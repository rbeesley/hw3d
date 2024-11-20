#pragma once
#include <string>
#include <unordered_map>

#define WINDOWS_MESSAGE_MAP_
#include "AtumWindows.hpp"

class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::string operator()(DWORD msg, LPARAM lParam, WPARAM wParam) const;
private:
	const std::unordered_map<DWORD, std::string> map_;
};
