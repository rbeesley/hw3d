#pragma once

#include <bitset>
#include <optional>
#include <queue>

#define KEYBOARD_
#include "AtumWindows.h"

class Keyboard
{
public:
	Keyboard() = default;
	~Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	Keyboard(const Keyboard&&) = delete;
	Keyboard& operator=(const Keyboard&&) = delete;

public:
	class Event
	{
	public:
		enum class EventType : std::uint8_t
		{
			PRESS,
			RELEASE,
			INVALID
		};
		using enum EventType;

	private:
		EventType eventType_;
		unsigned char code_;

	public:
		Event() = delete;
		Event(const EventType eventType, const unsigned char code) noexcept
			:
			eventType_(eventType),
			code_(code)
		{}

		[[nodiscard]] bool isPress() const noexcept
		{
			return eventType_ == PRESS;
		}

		[[nodiscard]] bool isRelease() const noexcept
		{
			return eventType_ == RELEASE;
		}

		[[nodiscard]] unsigned char getCode() const noexcept
		{
			return code_;
		}
	};

	// key event and char event management
private:
	template<typename T>
	static void trimBuffer(std::queue<T>& buffer) noexcept;

	// key event management
private:
	std::optional<Event> readKey() noexcept;
	[[nodiscard]] bool isKeyEmpty() const noexcept;
	void clearEventBuffer() noexcept;
public:
	[[nodiscard]] bool isKeyPressed(unsigned char keyCode) const noexcept;
	void onKeyPressed(unsigned char keyCode) noexcept;
	void onKeyReleased(unsigned char keyCode) noexcept;
	void clearState() noexcept;

	// char event management
private:
	std::optional<char> readChar() noexcept;
	[[nodiscard]] bool isCharEmpty() const noexcept;
	void clearCharBuffer() noexcept;
	void clear() noexcept;
public:
	void onChar(unsigned char character) noexcept;

	// auto repeat control
public:
	void enableAutorepeat() noexcept;
	void disableAutorepeat() noexcept;
	[[nodiscard]] bool isAutorepeatEnabled() const noexcept;

private:
	static constexpr unsigned int NUMBER_OF_KEYS = 256u;
	static constexpr unsigned int BUFFER_SIZE = 16u;
	bool autorepeatEnabled_ = false;
	std::bitset<NUMBER_OF_KEYS> keyState_;
	std::queue<Event> eventBuffer_;
	std::queue<char> charBuffer_;
};
