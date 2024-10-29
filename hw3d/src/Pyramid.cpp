#include "Pyramid.h"
#include "BindableIncludes.h"
#include "Cone.h"


pyramid::pyramid(graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& distance_distribution,									// rdist
	std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
	std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
	std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution	// odist 
)
	:
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
		struct color
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};

		struct vertex
		{
			dx::XMFLOAT3 pos;
			color color;
		};
		//auto model = cone::make_tessellated<vertex>(4);

		// set vertex colors for mesh
		//model.vertices()[0].color = { 255,0,0 };
		//model.vertices()[1].color = { 255,255,0 };
		//model.vertices()[2].color = { 0,255,0 };
		//model.vertices()[3].color = { 0,0,255 };
		//model.vertices()[4].color = { 63,63,63 };
		//model.vertices()[5].color = { 191,191,191 };

		// Define vertex colors
		std::vector<color> new_colors = {
			{255, 63, 63, 255},
			{127, 237, 16, 255},
			{0, 191, 191, 255},
			{127, 16, 237, 255},
			{0, 0, 0, 255},
			{255, 255, 255, 255}
		};

		// Lambda to set colors
		auto set_colors = [new_colors](std::vector<vertex>& vertices) {
			for (size_t i = 0; i < vertices.size() && i < new_colors.size(); ++i) {
				vertices[i].color = new_colors[i];
			}
		};

		// Create the model with vertices, indices, and the lambda
		auto model = cone::make_tessellated<vertex>(4, set_colors);

		// deform mesh linearly
		model.transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

		add_static_bind(std::make_unique<vertex_buffer>(graphics, model.vertices()));

		auto p_vertex_shader = std::make_unique<vertex_shader>(graphics, L"ColorBlendVS.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->get_byte_code();
		add_static_bind(std::move(p_vertex_shader));

		add_static_bind(std::make_unique<pixel_shader>(graphics, L"ColorBlendPS.cso"));

		add_static_index_buffer(std::make_unique<index_buffer>(graphics, model.indices()));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_descs =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		add_static_bind(std::make_unique<input_layout>(graphics, input_element_descs, p_vertex_shader_bytecode));

		add_static_bind(std::make_unique<topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		set_index_buffer_from_static_binds();
	}

	drawable::add_bind(std::make_unique<transform_constant_buffer>(graphics, *this));
}

void pyramid::update(float dt) noexcept
{
	roll_ += droll_ * dt;
	pitch_ += dpitch_ * dt;
	yaw_ += dyaw_ * dt;
	theta_ += dtheta_ * dt;
	phi_ += dphi_ * dt;
	rho_ += drho_ * dt;
}

DirectX::XMMATRIX pyramid::get_transform_xm() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		dx::XMMatrixTranslation(radius_distance_from_center_, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}