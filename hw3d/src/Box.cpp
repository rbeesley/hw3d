#include "Box.h"
#include "BindableBase.h"

box::box(graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& distance,
	std::uniform_real_distribution<float>& spherical_coordinate_position,
	std::uniform_real_distribution<float>& rotation_of_box,
	std::uniform_real_distribution<float>& spherical_coordinate_movement_of_box)
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
	if (!is_static_initialized())
	{
		struct vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;
		};
		const std::vector<vertex> vertices =
		{
			//    6-------7
			//   /|      /|
			//  2-------3 |
			//  | |     | |
			//  | 4-----|-5
			//  |/      |/
			//  0-------1

			{-1.0f, -1.0f, -1.0f}, // Bottom-left-front vertex
			{1.0f, -1.0f, -1.0f}, // Bottom-right-front vertex
			{-1.0f, 1.0f, -1.0f}, // Top-left-front vertex
			{1.0f, 1.0f, -1.0f}, // Top-right-front vertex
			{-1.0f, -1.0f, 1.0f}, // Bottom-left-back vertex
			{1.0f, -1.0f, 1.0f}, // Bottom-right-back vertex
			{-1.0f, 1.0f, 1.0f}, // Top-left-back vertex
			{1.0f, 1.0f, 1.0f}, // Top-right-back vertex
		};
		add_static_bind(std::make_unique<vertex_buffer>(graphics, vertices));

		auto p_vertex_shader = std::make_unique<vertex_shader>(graphics, L"VertexShader.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->get_byte_code();
		add_static_bind(std::move(p_vertex_shader));

		add_static_bind(std::make_unique<pixel_shader>(graphics, L"PixelShader.cso"));

		const std::vector<unsigned short> indices =
		{
			//         2------6
			//         | 4  //|
			//	       |  //  |
			//	       |//  5 |
			//  2------3------7------6------2
			//  |\\  1 |\\  3 |\\  7 | 9  //|
			//  |  \\  |  \\  |  \\  |  //  |
			//  | 0  \\| 2  \\| 6  \\|//  8 |
			//  0------1------5------4------0
			//         |\\ 11 |
			//	       |  \\  |
			//	       | 10 \\|
			//	       0------4

			0, 2, 1, 2, 3, 1, // Front
			1, 3, 5, 3, 7, 5, // Right
			2, 6, 3, 3, 6, 7, // Top
			4, 5, 7, 4, 7, 6, // Back
			0, 4, 2, 2, 4, 6, // Left
			0, 1, 4, 1, 5, 4, // Bottom
		};
		add_static_index_buffer(std::make_unique<index_buffer>(graphics, indices));

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
