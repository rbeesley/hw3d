#pragma once
#include <DirectXMath.h>
#include <random>

#include "Drawable.h"
#include "Graphics.h"

class box : public drawable<box>
{
public:
	box(graphics& graphics, std::mt19937& rng,
		std::uniform_real_distribution<float>& distance,
		std::uniform_real_distribution<float>& spherical_coordinate_position,
		std::uniform_real_distribution<float>& rotation_of_box,
		std::uniform_real_distribution<float>& spherical_coordinate_movement_of_box);

	box(const box&) = delete;
	box& operator=(const box&) = delete;
	box(const box&&) = delete;
	box& operator=(const box&&) = delete;

	void update(float dt) noexcept override;
	DirectX::XMMATRIX get_transform_xm() const noexcept override;
private:
	// Positional
	float radius_distance_from_center_;
	float roll_ = 0.0f;
	float pitch_ = 0.0f;
	float yaw_ = 0.0f;
	float theta_;
	float phi_;
	float rho_;

	// Speed (delta/s)
	float droll_;
	float dpitch_;
	float dyaw_;
	float dtheta_;
	float dphi_;
	float drho_;
};
