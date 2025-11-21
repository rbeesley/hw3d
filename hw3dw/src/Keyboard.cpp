#include "Keyboard.hpp"

#include <optional>

#include "Logging.hpp"

#if defined(LOG_WINDOW_MESSAGES) || defined(LOG_WINDOW_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include "WindowsMessageMap.hpp"
const static WindowsMessageMap windowsMessageMap;
#endif

#if defined(LOG_KEYBOARD_MESSAGES) || defined(LOG_KEYBOARD_CHARS)  // defined in LoggingConfig.h
#include <format>
#include "VirtualKeyMap.h"

const static VirtualKeyMap virtualKeyMap;
#endif

bool Keyboard::autorepeatEnabled_ = false;

bool Keyboard::isKeyPressed(const unsigned char keyCode) const noexcept
{
	return keyState_[keyCode];
}

std::optional<Keyboard::Event> Keyboard::readKey() noexcept
{
	if (!eventBuffer_.empty())
	{
		const Event e = eventBuffer_.front();
		eventBuffer_.pop();
		return e;
	}
	return {};
}

bool Keyboard::isKeyEmpty() const noexcept
{
	return eventBuffer_.empty();
}

void Keyboard::clearEventBuffer() noexcept
{
	eventBuffer_ = std::queue<Event>();
}

LRESULT Keyboard::handleMsg([[maybe_unused]] HWND window, const UINT msg, const WPARAM wParam, const LPARAM lParam) noexcept
{
#ifdef LOG_WINDOW_MESSAGES
	PLOGV << windowsMessageMap(msg, lParam, wParam).c_str();
#endif
	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		// Filter keyboard autorepeat
		if (!(lParam & 0x40000000 /* previous key state */) || isAutorepeatEnabled())
		{
			onKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		onKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		onChar(static_cast<unsigned char>(wParam));
		break;
	default:
		break;
	}
	return 0;
}

bool Keyboard::isCharEmpty() const noexcept
{
	return charBuffer_.empty();
}

void Keyboard::clearCharBuffer() noexcept
{
	charBuffer_ = std::queue<char>();
}

void Keyboard::clear() noexcept
{
	clearEventBuffer();
	clearCharBuffer();
}

void Keyboard::enableAutorepeat() noexcept
{
	autorepeatEnabled_ = true;
}

void Keyboard::disableAutorepeat() noexcept
{
	autorepeatEnabled_ = false;
}

bool Keyboard::isAutorepeatEnabled() noexcept
{
	return autorepeatEnabled_;
}

void Keyboard::onKeyPressed(unsigned char keyCode) noexcept
{
	keyState_[keyCode] = true;
	eventBuffer_.emplace(Event::EventType::PRESS, keyCode);
	trimBuffer(eventBuffer_);
#ifdef LOG_KEYBOARD_MESSAGES // defined in LoggingConfig.h
	PLOGV << "keydown: " << virtualKeyMap(keyCode);
#endif
}

void Keyboard::onKeyReleased(unsigned char keyCode) noexcept
{
	keyState_[keyCode] = false;
	eventBuffer_.emplace(Event::EventType::RELEASE, keyCode);
	trimBuffer(eventBuffer_);
#ifdef LOG_KEYBOARD_MESSAGES // defined in LoggingConfig.h
	PLOGV << "keyup: " << virtualKeyMap(keyCode);
#endif
}

void Keyboard::onChar(unsigned char character) noexcept
{
	charBuffer_.emplace(character);
	trimBuffer(charBuffer_);
#ifdef LOG_KEYBOARD_CHARS // defined in LoggingConfig.h
	PLOGV << "char: " << character;
#endif
}

void Keyboard::clearState() noexcept
{
	keyState_.reset();
}

template <typename T>
void Keyboard::trimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > BUFFER_SIZE)
	{
		buffer.pop();
	}
}

std::optional<char> Keyboard::readChar() noexcept
{
	if (!charBuffer_.empty())
	{
		const char charCode = charBuffer_.front();
		charBuffer_.pop();
		return charCode;
	}
	else
	{
		return {};
	}
}
