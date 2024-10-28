#pragma once
#include "DrawableStaticStorage.h"

class pyramid : public drawable_static_storage<pyramid>
{
public:
	pyramid(graphics& graphics, std::mt19937& rng,
		std::uniform_real_distribution<float>& distance_distribution,									// rdist
		std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
		std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
		std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution	// odist 
	);
	void update(float dt) noexcept override;
	DirectX::XMMATRIX get_transform_xm() const noexcept override;
private:
	// positional
	float radius_distance_from_center_; // r
	float roll_ = 0.0f;
	float pitch_ = 0.0f;
	float yaw_ = 0.0f;
	float theta_;
	float phi_;
	float rho_; // chi

	// speed (delta/s)
	float droll_;
	float dpitch_;
	float dyaw_;
	float dtheta_;
	float dphi_;
	float drho_; // dchi
};