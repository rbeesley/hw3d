#include "Mouse.hpp"

#include <optional>

#include "Logging.hpp"

#if defined(LOG_MOUSE_MESSAGES) // defined in LoggingConfig.h
#include <format>
#include "VirtualKeyMap.h"

const static VirtualKeyMap virtualKeyMap;
#endif

using enum Mouse::Event::EventType;

void Mouse::trimBuffer() noexcept
{
	while (eventBuffer_.size() > bufferSize_)
	{
		eventBuffer_.pop();
	}
}

std::optional<Mouse::Event> Mouse::read() noexcept
{
	if (!eventBuffer_.empty())
	{
		const Event e = eventBuffer_.front();
		eventBuffer_.pop();
		return e;
	}
	return {};
}

bool Mouse::isEmpty() const noexcept
{
	return eventBuffer_.empty();
}

void Mouse::clear() noexcept
{
	eventBuffer_ = std::queue<Event>();
}

std::pair<int, int> Mouse::getPos() const noexcept
{
	return { x_, y_ };
}

Mouse::Position Mouse::pos() const noexcept
{
	return Position{ x_, y_ };
}

int Mouse::getPosX() const noexcept
{
	return x_;
}

int Mouse::getPosY() const noexcept
{
	return y_;
}

bool Mouse::isInWindow() const noexcept
{
	return inWindow_;
}

bool Mouse::isLeftPressed() const noexcept
{
	return leftIsPressed_;
}

bool Mouse::isRightPressed() const noexcept
{
	return rightIsPressed_;
}

bool Mouse::isMiddlePressed() const noexcept
{
	return middleIsPressed_;
}

bool Mouse::isX1Pressed() const noexcept
{
	return x1IsPressed_;
}

bool Mouse::isX2Pressed() const noexcept
{
	return x2IsPressed_;
}

void Mouse::onMouseMove(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	eventBuffer_.emplace(MOVE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse move: x:" << x << " y:" << y;
#endif
}

void Mouse::onMouseLeave() noexcept
{
	inWindow_ = false;

	eventBuffer_.emplace(LEAVE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse leave window";
#endif
}

void Mouse::onMouseEnter(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	inWindow_ = true;

	eventBuffer_.emplace(ENTER, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse enter window: x:" << x << " y:" << y;
#endif
}

void Mouse::onLeftPressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	leftIsPressed_ = true;

	eventBuffer_.emplace(L_PRESS, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse left pressed";
#endif
}

void Mouse::onLeftReleased(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	leftIsPressed_ = false;

	eventBuffer_.emplace(L_RELEASE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse left released";
#endif
}

void Mouse::onRightPressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	rightIsPressed_ = true;

	eventBuffer_.emplace(R_PRESS, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse right pressed";
#endif
}

void Mouse::onRightReleased(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	rightIsPressed_ = false;

	eventBuffer_.emplace(R_RELEASE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse right released";
#endif
}

void Mouse::onMiddlePressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	middleIsPressed_ = true;

	eventBuffer_.emplace(M_PRESS, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse middle pressed";
#endif
}

void Mouse::onMiddleReleased(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	middleIsPressed_ = false;

	eventBuffer_.emplace(M_RELEASE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse middle released";
#endif
}

void Mouse::onX1Pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	x1IsPressed_ = true;

	eventBuffer_.emplace(X1_PRESS, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 4 pressed";
#endif
}

void Mouse::onX1Released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	x1IsPressed_ = false;

	eventBuffer_.emplace(X1_RELEASE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 4 released";
#endif
}

void Mouse::onX2Pressed(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	x2IsPressed_ = true;

	eventBuffer_.emplace(X2_PRESS, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 5 pressed";
#endif
}

void Mouse::onX2Released(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;
	x2IsPressed_ = false;

	eventBuffer_.emplace(X2_RELEASE, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse button 5 released";
#endif
}

void Mouse::onWheelUp(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	eventBuffer_.emplace(WHEEL_UP, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel up";
#endif
}

void Mouse::onWheelDown(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	eventBuffer_.emplace(WHEEL_DOWN, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel down";
#endif
}

void Mouse::onWheelRight(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	eventBuffer_.emplace(WHEEL_RIGHT, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel right";
#endif
}

void Mouse::onWheelLeft(const int x, const int y) noexcept
{
	x_ = x;
	y_ = y;

	eventBuffer_.emplace(WHEEL_LEFT, *this);
	trimBuffer();
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse wheel left";
#endif
}

void Mouse::onVWheelDelta(const int x, const int y, const int delta) noexcept
{
	vWheelDeltaCarry_ += delta;
	while (vWheelDeltaCarry_ >= WHEEL_DELTA)
	{
		vWheelDeltaCarry_ -= WHEEL_DELTA;
		onWheelUp(x, y);
	}
	while (vWheelDeltaCarry_ <= -WHEEL_DELTA)
	{
		vWheelDeltaCarry_ += WHEEL_DELTA;
		onWheelDown(x, y);
	}
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse v wheel delta: " << vWheelDeltaCarry_;
#endif
}

void Mouse::onHWheelDelta(const int x, const int y, const int delta) noexcept
{
	hWheelDeltaCarry_ += delta;
	while (hWheelDeltaCarry_ >= WHEEL_DELTA)
	{
		hWheelDeltaCarry_ -= WHEEL_DELTA;
		onWheelRight(x, y);
	}
	while (hWheelDeltaCarry_ <= -WHEEL_DELTA)
	{
		hWheelDeltaCarry_ += WHEEL_DELTA;
		onWheelLeft(x, y);
	}
#ifdef LOG_MOUSE_MESSAGES // defined in LoggingConfig.h
	PLOGV << "mouse h wheel delta: " << hWheelDeltaCarry_;
#endif
}
