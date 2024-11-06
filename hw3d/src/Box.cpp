#include "Box.h"

#include "AtumMath.h"
#include "BindableIncludes.h"
#include "Cube.h"

box::box(graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& distance_distribution,									// rdist
	std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
	std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
	std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution,	// odist
	std::uniform_real_distribution<float>& z_axis_distortion_distribution							// bdist
)
	: drawable_static_storage(),
	radius_distance_from_center_(distance_distribution(rng)),
	theta_(spherical_coordinate_position_distribution(rng)),
	phi_(spherical_coordinate_position_distribution(rng)),
	rho_(spherical_coordinate_position_distribution(rng)),
	droll_(rotation_of_drawable_distribution(rng)),
	dpitch_(rotation_of_drawable_distribution(rng)),
	dyaw_(rotation_of_drawable_distribution(rng)),
	dtheta_(spherical_coordinate_movement_of_drawable_distribution(rng)),
	dphi_(spherical_coordinate_movement_of_drawable_distribution(rng)),
	drho_(spherical_coordinate_movement_of_drawable_distribution(rng))
{
	namespace dx = DirectX;

	if (!is_static_initialized())
	{
		struct vertex
		{
			dx::XMFLOAT3 pos;
		};
		const auto model = cube::make<vertex>();

		add_static_bind(std::make_unique<vertex_buffer>(graphics, model.vertices()));

		auto p_vertex_shader = std::make_unique<vertex_shader>(graphics, L"ColorIndexVS.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->get_byte_code();
		add_static_bind(std::move(p_vertex_shader));

		add_static_bind(std::make_unique<pixel_shader>(graphics, L"ColorIndexPS.cso"));

		add_static_index_buffer(std::make_unique<index_buffer>(graphics, model.indices()));

		struct pixel_shader_constants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[8];
		};
		const pixel_shader_constants constant_buffer =
		{
			{
				{1.0f, 1.0f, 1.0f, 1.0f}, // White
				{0.0f, 1.0f, 0.0f, 1.0f}, // Green
				{1.0f, 0.0f, 0.0f, 1.0f}, // Red
				{1.0f, 1.0f, 0.0f, 1.0f}, // Yellow
				{0.0f, 0.0f, 1.0f, 1.0f}, // Blue
				{1.0f, 0.5f, 0.0f, 1.0f}, // Orange
				{1.0f, 0.0f, 1.0f, 1.0f}, // Magenta
				{0.0f, 1.0f, 1.0f, 1.0f}, // Cyan
			}
		};
		add_static_bind(std::make_unique<pixel_constant_buffer<pixel_shader_constants>>(graphics, constant_buffer));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_descs =
		{
			{
				.SemanticName = "POSITION", // Input in the vertex shader
				.SemanticIndex = 0u,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0u,
				.AlignedByteOffset = 0u,
				.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0u
			},
		};
		add_static_bind(std::make_unique<input_layout>(graphics, input_element_descs, p_vertex_shader_bytecode));

		add_static_bind(std::make_unique<topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		set_index_buffer_from_static_binds();
	}

	drawable::add_bind(std::make_unique<transform_constant_buffer>(graphics, *this));

	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(
		&model_transform_,
		dx::XMMatrixScaling(1.0f, 1.0f, z_axis_distortion_distribution(rng))
	);
}

void box::update(const float dt) noexcept
{
	roll_ += wrap_angle(droll_ * dt);
	pitch_ += wrap_angle(dpitch_ * dt);
	yaw_ += wrap_angle(dyaw_ * dt);
	theta_ += wrap_angle(dtheta_ * dt);
	phi_ += wrap_angle(dphi_ * dt);
	rho_ += wrap_angle(drho_ * dt);
}

DirectX::XMMATRIX box::get_transform_xm() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		DirectX::XMMatrixTranslation(radius_distance_from_center_, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
