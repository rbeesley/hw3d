#include "Sheet.h"
#include "BindableIncludes.h"
#include "Plane.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"


sheet::sheet(graphics& graphics,
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
		struct texture_position
		{
			float u;
			float v;
		};

		struct vertex
		{
			dx::XMFLOAT3 pos;
			texture_position texture_position;
		};

		std::vector<texture_position> textures = {
			{ 0.0f,0.0f },
			{ 1.0f,0.0f },
			{ 0.0f,1.0f },
			{ 1.0f,1.0f }
		};

		// Lambda to set textures
		auto set_texture = [textures](std::vector<vertex>& vertices) {
			for (size_t i = 0; i < vertices.size() && i < textures.size(); ++i) {
				vertices[i].texture_position = textures[i];
			}
			};

		const auto model = plane::make<vertex>(set_texture);

		add_static_bind(std::make_unique<texture>(graphics, surface::from_file(L"kappa50.png")));

		add_static_bind(std::make_unique<vertex_buffer>(graphics, model.vertices()));

		add_static_bind(std::make_unique<sampler>(graphics));

		auto p_vertex_shader = std::make_unique<vertex_shader>(graphics, L"TextureVS.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->get_byte_code();
		add_static_bind(std::move(p_vertex_shader));

		add_static_bind(std::make_unique<pixel_shader>(graphics, L"TexturePS.cso"));

		add_static_index_buffer(std::make_unique<index_buffer>(graphics, model.indices()));

		constexpr D3D11_INPUT_ELEMENT_DESC position_desc = {
			.SemanticName = "Position",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};

		constexpr D3D11_INPUT_ELEMENT_DESC texcoord_desc = {
			.SemanticName = "TexCoord",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};

		const std::vector input_element_descs =
		{
			position_desc,
			texcoord_desc
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

void sheet::update(const float dt) noexcept
{
	roll_ += wrap_angle(droll_ * dt);
	pitch_ += wrap_angle(dpitch_ * dt);
	yaw_ += wrap_angle(dyaw_ * dt);
	theta_ += wrap_angle(dtheta_ * dt);
	phi_ += wrap_angle(dphi_ * dt);
	rho_ += wrap_angle(drho_ * dt);
}

DirectX::XMMATRIX sheet::get_transform_xm() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		dx::XMMatrixTranslation(radius_distance_from_center_, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}