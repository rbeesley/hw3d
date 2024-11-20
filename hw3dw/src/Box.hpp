#pragma once
#include <DirectXMath.h>
#include <random>

#include "DrawableStaticStorage.hpp"
#include "Graphics.hpp"

class Box : public DrawableStaticStorage<Box>
{
public:
	Box(Graphics& graphics, std::mt19937& rng,
		std::uniform_real_distribution<float>& distance_distribution,									// rdist
		std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
		std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
		std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution,	// odist
		std::uniform_real_distribution<float>& z_axis_distortion_distribution							// bdist
	);

	Box(const Box&) = delete;
	Box& operator=(const Box&) = delete;
	Box(const Box&&) = delete;
	Box& operator=(const Box&&) = delete;

	void update(float dt) noexcept override;
	DirectX::XMMATRIX getTransformXm() const noexcept override;
private:
	// Positional
	float radiusDistanceFromCenter_;
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

	// model transform
	DirectX::XMFLOAT3X3 model_transform_;
};
