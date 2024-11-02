#pragma once
#include <string>
#include <unordered_map>

#define WINDOWS_MESSAGE_MAP_
#include "AtumWindows.h"

class windows_message_map
{
public:
	windows_message_map();
	std::string operator()(DWORD msg, LPARAM l_param, WPARAM w_param) const;
private:
	const std::unordered_map<DWORD, std::string> map_;
};
