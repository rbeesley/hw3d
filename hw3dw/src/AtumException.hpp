#pragma once
#include <exception>
#include <string>

class AtumException : public std::exception {
public:
	AtumException(int line, const char* file) noexcept;
	AtumException() = delete;

	const char* what() const noexcept override;
	virtual const char* getType() const noexcept;
	int getLine() const noexcept;
	const std::string& getFile() const noexcept;
	std::string getOriginString() const noexcept;
private:
	int line_;
	std::string file_;
protected:
	mutable std::string whatBuffer_;
};
