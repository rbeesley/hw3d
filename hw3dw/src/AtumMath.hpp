#pragma once
#include <cmath>
#include <numbers>

constexpr float castDoubleToFloat(const double value)
{
	return static_cast<float>(value);
}

constexpr double PI_D = std::numbers::pi;
constexpr double TWO_PI_D = 2.0 * std::numbers::pi;
constexpr double ONE_DIV_PI_D = 1.0 / std::numbers::pi;
constexpr double ONE_DIV_2_PI_D = 1.0 / (2.0 * std::numbers::pi);
constexpr double PI_DIV_2_D = std::numbers::pi / 2.0;
constexpr double PI_DIV_4_D = std::numbers::pi / 4.0;

constexpr float PI = castDoubleToFloat(PI_D);
constexpr float TWO_PI = castDoubleToFloat(TWO_PI_D);
constexpr float ONE_DIV_PI = castDoubleToFloat(ONE_DIV_PI_D);
constexpr float ONE_DIV_2_PI = castDoubleToFloat(ONE_DIV_2_PI_D);
constexpr float PI_DIV_2 = castDoubleToFloat(PI_DIV_2_D);
constexpr float PI_DIV_4 = castDoubleToFloat(PI_DIV_4_D);

template <typename T>
constexpr auto sq(const T& x)
{
	return x * x;
}

template<typename T>
T wrapAngle(T theta)
{
	const auto modded = fmod(theta, TWO_PI_D);
	return (modded > PI_D) ?
		static_cast<T>(modded - TWO_PI_D) :
		static_cast<T>(modded);
}

template<typename T>
constexpr T interpolated(const T& src, const T& dst, float alpha)
{
	return src + (dst - src) * alpha;
}

template<typename TIn, typename TOut = float>
constexpr double map(const TIn in, const TIn inMin, const TIn inMax, const TIn outMin, const TIn outMax)
{
	return (static_cast<TOut>(in) - static_cast<TOut>(inMin)) * (static_cast<TOut>(outMax) - static_cast<TOut>(outMin)) / (static_cast<TOut>(inMax) - static_cast<TOut>(inMin)) + static_cast<TOut>(outMin);
}

template<typename T>
constexpr T toRad(T deg)
{
	return static_cast<T>(deg * PI_D / 180.0);
}

template<typename T>
constexpr T toDeg(T rad)
{
	return static_cast<T>(rad / PI_D * 180.0);
}
