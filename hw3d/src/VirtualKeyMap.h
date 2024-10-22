#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>

class virtual_key_map
{
public:
	virtual_key_map();

	~virtual_key_map() = default;
	virtual_key_map(const virtual_key_map&) = delete;
	virtual_key_map& operator=(const virtual_key_map&) = delete;
	virtual_key_map(const virtual_key_map&&) = delete;
	virtual_key_map& operator=(const virtual_key_map&&) = delete;

	std::string operator()(unsigned char virtual_key_code) const;
private:
	const std::unordered_map<DWORD, std::string> map_;
};
