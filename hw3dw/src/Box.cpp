#include "Box.hpp"

#include "AtumMath.hpp"
#include "BindableIncludes.hpp"
#include "Cube.hpp"

Box::Box(Graphics& graphics,
         std::mt19937& rng,
         std::uniform_real_distribution<float>& distance_distribution,									// rdist
         std::uniform_real_distribution<float>& spherical_coordinate_position_distribution,				// adist
         std::uniform_real_distribution<float>& rotation_of_drawable_distribution,						// ddist
         std::uniform_real_distribution<float>& spherical_coordinate_movement_of_drawable_distribution,	// odist
         std::uniform_real_distribution<float>& z_axis_distortion_distribution							// bdist
)
	: DrawableStaticStorage(),
	radiusDistanceFromCenter_(distance_distribution(rng)),
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

	if (!isStaticInitialized())
	{
		struct vertex
		{
			dx::XMFLOAT3 pos;
		};
		const auto model = Cube::make<vertex>();

		addStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices()));

		auto p_vertex_shader = std::make_unique<VertexShader>(graphics, L"ColorIndexVS.cso");
		auto p_vertex_shader_bytecode = p_vertex_shader->getByteCode();
		addStaticBind(std::move(p_vertex_shader));

		addStaticBind(std::make_unique<PixelShader>(graphics, L"ColorIndexPS.cso"));

		addStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices()));

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
		addStaticBind(std::make_unique<PixelConstantBuffer<pixel_shader_constants>>(graphics, constant_buffer));

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
		addStaticBind(std::make_unique<InputLayout>(graphics, input_element_descs, p_vertex_shader_bytecode));

		addStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		setIndexBufferFromStaticBinds();
	}

	Drawable::addBind(std::make_unique<TransformConstantBuffer>(graphics, *this));

	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(
		&model_transform_,
		dx::XMMatrixScaling(1.0f, 1.0f, z_axis_distortion_distribution(rng))
	);
}

void Box::update(const float dt) noexcept
{
	roll_ += wrapAngle(droll_ * dt);
	pitch_ += wrapAngle(dpitch_ * dt);
	yaw_ += wrapAngle(dyaw_ * dt);
	theta_ += wrapAngle(dtheta_ * dt);
	phi_ += wrapAngle(dphi_ * dt);
	rho_ += wrapAngle(drho_ * dt);
}

DirectX::XMMATRIX Box::getTransformXm() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		DirectX::XMMatrixTranslation(radiusDistanceFromCenter_, 0.0f, 0.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		DirectX::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
