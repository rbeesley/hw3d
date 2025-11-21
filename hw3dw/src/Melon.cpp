#include "Melon.hpp"

#include "BindableIncludes.hpp"
#include "Sphere.hpp"

Melon::Melon(Graphics& graphics,
             std::mt19937& rng,
             std::uniform_real_distribution<float>& distanceDistribution,								// rdist
             std::uniform_real_distribution<float>& sphericalCoordinatePositionDistribution,			// adist
             std::uniform_real_distribution<float>& rotationOfDrawableDistribution,						// ddist
             std::uniform_real_distribution<float>& sphericalCoordinateMovementOfDrawableDistribution,	// odist
             std::uniform_int_distribution<int>& latitudeDistribution,
             std::uniform_int_distribution<int>& longitudeDistribution)
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
		auto vertexShader = std::make_unique<VertexShader>(graphics, L"ColorIndexVS.cso");
		auto vertexShaderBytecode = vertexShader->getByteCode();
		addStaticBind(std::move(vertexShader));

		addStaticBind(std::make_unique<PixelShader>(graphics, L"ColorIndexPS.cso"));

		struct PixelShaderConstraints
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} faceColors[8];
		};

		const PixelShaderConstraints constantBuffer =
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

		addStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstraints>>(graphics, constantBuffer));

		constexpr D3D11_INPUT_ELEMENT_DESC positionDesc = {
			.SemanticName = "Position",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};

		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs =
		{
			positionDesc
		};

		addStaticBind(std::make_unique<InputLayout>(graphics, inputElementDescs, vertexShaderBytecode));

		addStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = Sphere::makeTessellated<Vertex>(latitudeDistribution(rng), longitudeDistribution(rng));

	// deform vertices of model by linear transformation
	model.transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

	Drawable::addBind(std::make_unique<VertexBuffer>(graphics, model.vertices()));

	Drawable::addIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices()));

	Drawable::addBind(std::make_unique<TransformConstantBuffer>(graphics, *this));
}

void Melon::update(const float dt) noexcept
{
	roll_ += wrapAngle(droll_ * dt);
	pitch_ += wrapAngle(dpitch_ * dt);
	yaw_ += wrapAngle(dyaw_ * dt);
	theta_ += wrapAngle(dtheta_ * dt);
	phi_ += wrapAngle(dphi_ * dt);
	rho_ += wrapAngle(drho_ * dt);
}

DirectX::XMMATRIX Melon::getTransformXm() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		dx::XMMatrixTranslation(radiusDistanceFromCenter_, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}