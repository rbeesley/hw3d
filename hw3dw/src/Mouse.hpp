#pragma once
#include <optional>
#include <queue>
#include <utility>

#define MOUSE_
#include "AtumWindows.hpp"

class Mouse
{
public:
	Mouse() = default;
	~Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	Mouse(const Mouse&&) = delete;
	Mouse& operator=(const Mouse&&) = delete;

public:
	class Event
	{
	public:
		enum class EventType : std::uint8_t
		{
			L_PRESS,
			L_RELEASE,
			L_DOUBLE,
			R_PRESS,
			R_RELEASE,
			R_DOUBLE,
			M_PRESS,
			M_RELEASE,
			M_DOUBLE,
			X1_PRESS,
			X1_RELEASE,
			X2_PRESS,
			X2_RELEASE,
			WHEEL_UP,
			WHEEL_DOWN,
			WHEEL_RIGHT,
			WHEEL_LEFT,
			MOVE,
			ENTER,
			LEAVE
		};
		using enum EventType;

	private:
		EventType eventType_;
		int x_;
		int y_;
		bool leftIsPressed_;
		bool rightIsPressed_;
		bool middleIsPressed_;
		bool x1IsPressed_;
		bool x2IsPressed_;

	public:
		Event() = delete;
		Event(const EventType eventType, const Mouse& parent) noexcept
			:
			eventType_(eventType),
			x_(parent.x_),
			y_(parent.y_),
			leftIsPressed_(parent.leftIsPressed_),
			rightIsPressed_(parent.rightIsPressed_),
			middleIsPressed_(parent.middleIsPressed_),
			x1IsPressed_(parent.x1IsPressed_),
			x2IsPressed_(parent.x2IsPressed_)
		{}

		[[nodiscard]] EventType get_type() const noexcept
		{
			return eventType_;
		}

		[[nodiscard]] std::pair<int, int> get_pos() const noexcept
		{
			return { x_, y_ };
		}

		[[nodiscard]] int get_pos_x() const noexcept
		{
			return x_;
		}

		[[nodiscard]] int get_pos_y() const noexcept
		{
			return y_;
		}

		[[nodiscard]] bool is_left_pressed() const noexcept
		{
			return leftIsPressed_;
		}

		[[nodiscard]] bool is_middle_pressed() const noexcept
		{
			return middleIsPressed_;
		}

		[[nodiscard]] bool is_right_pressed() const noexcept
		{
			return rightIsPressed_;
		}

		[[nodiscard]] bool is_x1_pressed() const noexcept
		{
			return x1IsPressed_;
		}

		[[nodiscard]] bool is_x2_pressed() const noexcept
		{
			return x2IsPressed_;
		}
	};

private:
	void trimBuffer() noexcept;
	std::optional<Event> read() noexcept;
	[[nodiscard]] bool isEmpty() const noexcept;
	void clear() noexcept;

public:
	struct Position { int x, y; };
	[[nodiscard]] std::pair<int, int> getPos() const noexcept;
	[[nodiscard]] Position pos() const noexcept;
	[[nodiscard]] int getPosX() const noexcept;
	[[nodiscard]] int getPosY() const noexcept;
	[[nodiscard]] bool isInWindow() const noexcept;
	[[nodiscard]] bool isLeftPressed() const noexcept;
	[[nodiscard]] bool isRightPressed() const noexcept;
	[[nodiscard]] bool isMiddlePressed() const noexcept;
	[[nodiscard]] bool isX1Pressed() const noexcept;
	[[nodiscard]] bool isX2Pressed() const noexcept;
	void onMouseMove(int x, int y) noexcept;
	void onMouseLeave() noexcept;
	void onMouseEnter(int x, int y) noexcept;
	void onLeftPressed(int x, int y) noexcept;
	void onLeftReleased(int x, int y) noexcept;
	void onRightPressed(int x, int y) noexcept;
	void onRightReleased(int x, int y) noexcept;
	void onMiddlePressed(int x, int y) noexcept;
	void onMiddleReleased(int x, int y) noexcept;
	void onX1Pressed(int x, int y) noexcept;
	void onX1Released(int x, int y) noexcept;
	void onX2Pressed(int x, int y) noexcept;
	void onX2Released(int x, int y) noexcept;
	void onWheelUp(int x, int y) noexcept;
	void onWheelDown(int x, int y) noexcept;
	void onWheelRight(int x, int y) noexcept;
	void onWheelLeft(int x, int y) noexcept;
	void onVWheelDelta(int x, int y, int delta) noexcept;
	void onHWheelDelta(int x, int y, int delta) noexcept;

private:
	static constexpr unsigned int bufferSize_ = 16u;
	int x_ = 0;
	int y_ = 0;
	bool inWindow_ = false;
	bool leftIsPressed_ = false;
	bool rightIsPressed_ = false;
	bool middleIsPressed_ = false;
	bool x1IsPressed_ = false;
	bool x2IsPressed_ = false;
	int vWheelDeltaCarry_ = 0;
	int hWheelDeltaCarry_ = 0;
	std::queue<Event> eventBuffer_;
};
