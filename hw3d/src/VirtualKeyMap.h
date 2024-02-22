#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>

class virtual_key_map
{
public:
	virtual_key_map();
	std::string operator()(unsigned char virtual_key_code) const;
private:
	const std::unordered_map<DWORD, std::string> map_;
};
