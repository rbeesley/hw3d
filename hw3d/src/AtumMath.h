#pragma once
#include <cmath>
#include <numbers>

constexpr float cast_double_to_float(const double value)
{
	return static_cast<float>(value);
}

constexpr double PI_D = std::numbers::pi;
constexpr double TWOPI_D = 2.0 * std::numbers::pi;
constexpr double ONEDIVPI_D = 1.0 / std::numbers::pi;
constexpr double ONEDIV2PI_D = 1.0 / (2.0 * std::numbers::pi);
constexpr double PIDIV2_D = std::numbers::pi / 2.0;
constexpr double PIDIV4_D = std::numbers::pi / 4.0;

constexpr float PI = cast_double_to_float(PI_D);
constexpr float TWOPI = cast_double_to_float(TWOPI_D);
constexpr float ONEDIVPI = cast_double_to_float(ONEDIVPI_D);
constexpr float ONEDIV2PI = cast_double_to_float(ONEDIV2PI_D);
constexpr float PIDIV2 = cast_double_to_float(PIDIV2_D);
constexpr float PIDIV4 = cast_double_to_float(PIDIV4_D);

template <typename T>
constexpr auto sq(const T& x)
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta)
{
	const auto modded = fmod(theta, TWOPI_D);
	return (modded > PI_D) ?
		static_cast<T>(modded - TWOPI_D) :
		static_cast<T>(modded);
}

template<typename T>
constexpr T interpolated(const T& src, const T& dst, float alpha)
{
	return src + (dst - src) * alpha;
}

template<typename TIn, typename TOut = float>
constexpr double map(const TIn in, const TIn in_min, const TIn in_max, const TIn out_min, const TIn out_max)
{
	return (static_cast<TOut>(in) - static_cast<TOut>(in_min)) * (static_cast<TOut>(out_max) - static_cast<TOut>(out_min)) / (static_cast<TOut>(in_max) - static_cast<TOut>(in_min)) + static_cast<TOut>(out_min);
}

template<typename T>
constexpr T to_rad(T deg)
{
	return static_cast<T>(deg * PI_D / 180.0);
}

template<typename T>
constexpr T to_deg(T rad)
{
	return static_cast<T>(rad / PI_D * 180.0);
}
