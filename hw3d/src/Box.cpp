#include "Box.h"

#include "BindableBase.h"
#include "Cone.h"
#include "Cube.h"
#include "Cylinder.h"
#include "Plane.h"
#include "Sphere.h"

box::box(graphics& graphics,
         std::mt19937& rng,
         std::uniform_real_distribution<float>& distance,								// rdist
         std::uniform_real_distribution<float>& spherical_coordinate_position,			// adist
         std::uniform_real_distribution<float>& rotation_of_box,						// ddist
         std::uniform_real_distribution<float>& spherical_coordinate_movement_of_box)	// odist
	: drawable(),
	  radius_distance_from_center_(distance(rng)),
	  theta_(spherical_coordinate_position(rng)),
	  phi_(spherical_coordinate_position(rng)),
	  rho_(spherical_coordinate_position(rng)),
	  droll_(rotation_of_box(rng)),
	  dpitch_(rotation_of_box(rng)),
	  dyaw_(rotation_of_box(rng)),
	  dtheta_(spherical_coordinate_movement_of_box(rng)),
	  dphi_(spherical_coordinate_movement_of_box(rng)),
	  drho_(spherical_coordinate_movement_of_box(rng))
{
	namespace dx = DirectX;

	if (!is_static_initialized())
	{
		struct vertex
		{
			dx::XMFLOAT3 pos;
		};

		auto model = cube::make<vertex>();
		model.transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.0f));

		add_static_bind(std::make_unique<vertex_buffer>(graphics, model.vertices()));

		auto p_vertex_shader = std::make_unique<vertex_shader>(graphics, L"VertexShader.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->get_byte_code();
		add_static_bind(std::move(p_vertex_shader));

		add_static_bind(std::make_unique<pixel_shader>(graphics, L"PixelShader.cso"));

		add_static_index_buffer(std::make_unique<index_buffer>(graphics, model.indices()));

		struct constant_buffer_struct
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const constant_buffer_struct constant_buffer =
		{
			{
				{1.0f, 1.0f, 1.0f, 1.0f}, // White
				{0.0f, 1.0f, 0.0f, 1.0f}, // Green
				{1.0f, 0.0f, 0.0f, 1.0f}, // Red
				{1.0f, 1.0f, 0.0f, 1.0f}, // Yellow
				{0.0f, 0.0f, 1.0f, 1.0f}, // Blue
				{1.0f, 0.5f, 0.0f, 1.0f}, // Orange
			}
		};
		add_static_bind(std::make_unique<pixel_constant_buffer<constant_buffer_struct>>(graphics, constant_buffer));

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

	drawable_base::add_bind(std::make_unique<transform_constant_buffer>(graphics, *this));
}

void box::update(const float dt) noexcept
{
	roll_ += droll_ * dt;
	pitch_ += dpitch_ * dt;
	yaw_ += dyaw_ * dt;
	theta_ += dtheta_ * dt;
	phi_ += dphi_ * dt;
	rho_ += drho_ * dt;
}

DirectX::XMMATRIX box::get_transform_xm() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		DirectX::XMMatrixTranslation(radius_distance_from_center_, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
