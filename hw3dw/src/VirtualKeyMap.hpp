#pragma once
#include <string>
#include <unordered_map>

#include "AtumWindows.hpp"

class VirtualKeyMap
{
public:
	VirtualKeyMap();

	~VirtualKeyMap() = default;
	VirtualKeyMap(const VirtualKeyMap&) = delete;
	VirtualKeyMap& operator=(const VirtualKeyMap&) = delete;
	VirtualKeyMap(const VirtualKeyMap&&) = delete;
	VirtualKeyMap& operator=(const VirtualKeyMap&&) = delete;

	std::string operator()(unsigned char virtualKeyCode) const;
private:
	const std::unordered_map<DWORD, std::string> map_;
};
