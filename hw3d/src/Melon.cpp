#include "Melon.h"

#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"

melon::melon(graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& distance_distribution,									// rdist
	std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
	std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
	std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution,	// odist
	std::uniform_int_distribution<int>& latitude_distribution,
	std::uniform_int_distribution<int>& longitude_distribution)
	:
	distance_(distance_distribution(rng)),
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
		auto p_vertex_shader = std::make_unique<vertex_shader>(graphics, L"ColorIndexVS.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->get_byte_code();
		add_static_bind(std::move(p_vertex_shader));
		add_static_bind(std::make_unique<pixel_shader>(graphics, L"ColorIndexPS.cso"));
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
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		add_static_bind(std::make_unique<pixel_constant_buffer<pixel_shader_constants>>(graphics, constant_buffer));
		const std::vector<D3D11_INPUT_ELEMENT_DESC> input_element_descs =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		add_static_bind(std::make_unique<input_layout>(graphics, input_element_descs, p_vertex_shader_bytecode));
		add_static_bind(std::make_unique<topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};
	auto model = sphere::make_tessellated<Vertex>(latitude_distribution(rng), longitude_distribution(rng));
	// deform vertices of model by linear transformation
	model.transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));
	drawable_base::add_bind(std::make_unique<vertex_buffer>(graphics, model.vertices()));
	drawable_base::add_index_buffer(std::make_unique<index_buffer>(graphics, model.indices()));
	drawable_base::add_bind(std::make_unique<transform_constant_buffer>(graphics, *this));
}
void melon::update(const float dt) noexcept
{
	roll_ += droll_ * dt;
	pitch_ += dpitch_ * dt;
	yaw_ += dyaw_ * dt;
	theta_ += dtheta_ * dt;
	phi_ += dphi_ * dt;
	rho_ += drho_ * dt;
}
DirectX::XMMATRIX melon::get_transform_xm() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		dx::XMMatrixTranslation(distance_, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}