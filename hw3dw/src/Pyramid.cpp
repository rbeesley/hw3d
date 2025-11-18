#include "Pyramid.hpp"
#include "BindableIncludes.hpp"
#include "Cone.hpp"

Pyramid::Pyramid(Graphics& graphics,
                 std::mt19937& rng,
                 std::uniform_real_distribution<float>& distanceDistribution,								// rdist
                 std::uniform_real_distribution<float>& sphericalCoordinatePositionDistribution,			// adist
                 std::uniform_real_distribution<float>& rotationOfDrawableDistribution,						// ddist
                 std::uniform_real_distribution<float>& sphericalCoordinateMovementOfDrawableDistribution	// odist
)
	:
	radiusDistanceFromCenter_(distanceDistribution(rng)),
	theta_(sphericalCoordinatePositionDistribution(rng)),
	phi_(sphericalCoordinatePositionDistribution(rng)),
	rho_(sphericalCoordinatePositionDistribution(rng)),
	droll_(rotationOfDrawableDistribution(rng)),
	dpitch_(rotationOfDrawableDistribution(rng)),
	dyaw_(rotationOfDrawableDistribution(rng)),
	dtheta_(sphericalCoordinateMovementOfDrawableDistribution(rng)),
	dphi_(sphericalCoordinateMovementOfDrawableDistribution(rng)),
	drho_(sphericalCoordinateMovementOfDrawableDistribution(rng))
{
	namespace dx = DirectX;

	if (!isStaticInitialized())
	{
		struct Color
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};

		struct Vertex
		{
			dx::XMFLOAT3 pos;
			Color color;
		};

		// Define vertex colors
		std::vector<Color> colors = {
			{255, 0, 0, 255},
			{255, 255, 0, 255},
			{0, 255, 0, 255},
			{0, 255, 255, 255},
			{0, 0, 255, 255},
			{255, 0, 255, 255},
		};

		// Lambda to set colors
		auto setColors = [colors](std::vector<Vertex>& vertices) {
			for (size_t i = 0; i < vertices.size() && i < colors.size(); ++i) {
				vertices[i].color = colors[i];
			}
		};

		// Create the model with vertices, indices, and the lambda
		auto model = Cone::makeTessellated<Vertex>(4, setColors);

		// deform mesh linearly
		model.transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

		addStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices()));

		auto vertexShader = std::make_unique<VertexShader>(graphics, L"ColorBlendVS.cso");
		auto vertexShaderBytecode = vertexShader->getByteCode();
		addStaticBind(std::move(vertexShader));

		addStaticBind(std::make_unique<PixelShader>(graphics, L"ColorBlendPS.cso"));

		addStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices()));

		constexpr D3D11_INPUT_ELEMENT_DESC positionDesc = {
			.SemanticName = "Position",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};

		constexpr D3D11_INPUT_ELEMENT_DESC colorDesc = {
			.SemanticName = "Color",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
			.InputSlot = 0,
			.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};

		const std::vector inputElementDescs =
		{
			positionDesc,
			colorDesc
		};

		addStaticBind(std::make_unique<InputLayout>(graphics, inputElementDescs, vertexShaderBytecode));

		addStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		setIndexBufferFromStaticBinds();
	}

	Drawable::addBind(std::make_unique<TransformConstantBuffer>(graphics, *this));
}

void Pyramid::update(const float dt) noexcept
{
	roll_ += wrapAngle(droll_ * dt);
	pitch_ += wrapAngle(dpitch_ * dt);
	yaw_ += wrapAngle(dyaw_ * dt);
	theta_ += wrapAngle(dtheta_ * dt);
	phi_ += wrapAngle(dphi_ * dt);
	rho_ += wrapAngle(drho_ * dt);
}

DirectX::XMMATRIX Pyramid::getTransformXm() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		dx::XMMatrixTranslation(radiusDistanceFromCenter_, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}