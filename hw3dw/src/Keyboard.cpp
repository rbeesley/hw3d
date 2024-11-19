#include "Keyboard.h"

#include <optional>

#include "Logging.h"

#if defined(LOG_KEYBOARD_MESSAGES) || defined(LOG_KEYBOARD_CHARS)  // defined in LoggingConfig.h
#include <format>
#include "VirtualKeyMap.h"

const static class VirtualKeyMap virtualKeyMap;
#endif

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

bool Keyboard::isAutorepeatEnabled() const noexcept
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
