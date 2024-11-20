#include "Sheet.hpp"
#include "BindableIncludes.hpp"
#include "Plane.hpp"
#include "Surface.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"


Sheet::Sheet(Graphics& graphics,
             std::mt19937& rng,
             std::uniform_real_distribution<float>& distanceDistribution,								// rdist
             std::uniform_real_distribution<float>& sphericalCoordinatePositionDistribution,				// adist
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
		struct TexturePosition
		{
			float u;
			float v;
		};

		struct Vertex
		{
			dx::XMFLOAT3 pos;
			TexturePosition texturePosition;
		};

		std::vector<TexturePosition> textures = {
			{ 0.0f,0.0f },
			{ 1.0f,0.0f },
			{ 0.0f,1.0f },
			{ 1.0f,1.0f }
		};

		// Lambda to set textures
		auto setTexture = [textures](std::vector<Vertex>& vertices) {
			for (size_t i = 0; i < vertices.size() && i < textures.size(); ++i) {
				vertices[i].texturePosition = textures[i];
			}
		};

		const auto model = Plane::make<Vertex>(setTexture);

		addStaticBind(std::make_unique<Texture>(graphics, Surface::fromFile(L"kappa50.png")));

		addStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices()));

		addStaticBind(std::make_unique<Sampler>(graphics));

		auto vertexShader = std::make_unique<VertexShader>(graphics, L"TextureVS.cso");
		auto vertexShaderBytecode = vertexShader->getByteCode();
		addStaticBind(std::move(vertexShader));

		addStaticBind(std::make_unique<PixelShader>(graphics, L"TexturePS.cso"));

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

		constexpr D3D11_INPUT_ELEMENT_DESC texcoordDesc = {
			.SemanticName = "TexCoord",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		};

		const std::vector inputElementDescs =
		{
			positionDesc,
			texcoordDesc
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

void Sheet::update(const float dt) noexcept
{
	roll_ += wrapAngle(droll_ * dt);
	pitch_ += wrapAngle(dpitch_ * dt);
	yaw_ += wrapAngle(dyaw_ * dt);
	theta_ += wrapAngle(dtheta_ * dt);
	phi_ += wrapAngle(dphi_ * dt);
	rho_ += wrapAngle(drho_ * dt);
}

DirectX::XMMATRIX Sheet::getTransformXm() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch_, yaw_, roll_) *
		dx::XMMatrixTranslation(radiusDistanceFromCenter_, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta_, phi_, rho_) *
		dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}