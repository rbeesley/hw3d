// ReSharper disable CppClangTidyClangDiagnosticExtraSemiStmt
#include "Graphics.h"
#include "DXErr.h"

#include "Logging.h"
#include "LoggingConfig.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <sstream>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#ifndef __cplusplus
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
#define __uuidof(iid) IID_##iid
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif

// Graphics exception macros, some with DXGI info

// Get the resulting error message and log the exception, no info manager
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT_NOINFO(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check, no info manager
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_NOINFO(hresult_call) if( FAILED(hresult = (hresult_call)) ) { throw graphics::hresult_exception(__LINE__, __FILE__, hresult); }

#if defined(DEBUG) || defined(_DEBUG)
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresult_call) info_manager_.set(); if( FAILED(hresult = (hresult_call)) ) { throw GFX_EXCEPT(hresult); }
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult), info_manager_.get_messages())
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) info_manager_.set(); (call); { auto v = info_manager_.get_messages(); if( !v.empty() ) { throw graphics::info_exception(__LINE__, __FILE__, v); }}
#else
// Get the resulting error message and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_EXCEPT(hresult) graphics::hresult_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check
// Has a dependency that HRESULT hresult; is allocated
#define GFX_THROW_INFO(hresult_call) if( FAILED(hresult = (hresult_call)) ) { throw graphics::hresult_exception(__LINE__, __FILE__, (hresult)); }
// Get the error for a device removed and log the exception
// Has a dependency that HRESULT hresult; is allocated and assigned a value
#define GFX_DEVICE_REMOVED_EXCEPTION(hresult) graphics::device_removed_exception(__LINE__, __FILE__, (hresult))
// Wrap graphics call with error check for a call which doesn't return an hresult
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

graphics::graphics(const HWND parent, int width, int height) :
	parent_(parent),
	width_(static_cast<float>(width)),
	height_(static_cast<float>(height))
{
	PLOGI << "Initializing Graphics";

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {
		.BufferDesc = {
			.Width = 0,
			.Height = 0,
			.RefreshRate = {
				.Numerator = 0,
				.Denominator = 0,
			},
			.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, // RGBA
			.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
			.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		},
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 1,
		.OutputWindow = parent_,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = 0,
	};

	UINT swap_create_flags = 0u;
#if defined(DEBUG) || defined(_DEBUG)
	swap_create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// For checking results of D3D functions
	HRESULT hresult;

	// Create device and front/back buffers, swap chain, and rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swap_create_flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		p_swap_chain_.GetAddressOf(),
		p_device_.GetAddressOf(),
		nullptr,
		p_device_context_.GetAddressOf()
	));

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif

	// Get the address of the back buffer
	wrl::ComPtr<ID3D11Resource> p_back_buffer;
	GFX_THROW_INFO(p_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &p_back_buffer));

	// Use the back buffer address to create the render target
	GFX_THROW_INFO(p_device_->CreateRenderTargetView(p_back_buffer.Get(), nullptr, p_target_view_.GetAddressOf()));

#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

void graphics::end_frame()
{
	HRESULT hresult;
#if defined(DEBUG) || defined(_DEBUG)
	info_manager_.set();
#endif
	if (FAILED(hresult = p_swap_chain_->Present(1u, 0u)))
	{
		if (hresult == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPTION(p_device_->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hresult);
		}
	}
	
}

void graphics::clear_buffer(const float red, const float green, const float blue) const
{
	const float color[] = { red, green, blue, 1.0f };
	p_device_context_->ClearRenderTargetView(p_target_view_.Get(), color);
}

// Experimental drawing code
void graphics::draw_test_triangle(const float angle, const float x, const float y, const float z)
{
	namespace wrl = Microsoft::WRL;
	HRESULT hresult;

	struct vertex
	{
		struct { float x; float y; float z; } pos;
	};

	// 4:3 aspect ratio correction
	const float aspect_ratio = height_ / width_;
	constexpr float scale_factor = 1.0f;

	// Create a vertex buffer structure
	const vertex vertices[] =
	{
		//    6-------7
		//   /|      /|
		//  2-------3 |
		//  | |     | |
		//  | 4-----|-5
		//  |/      |/
		//  0-------1

		{ -1.0f * scale_factor, -1.0f * scale_factor, -1.0f * scale_factor },	// Bottom-left-front vertex
		{ 1.0f * scale_factor, -1.0f * scale_factor, -1.0f * scale_factor },	// Bottom-right-front vertex
		{ -1.0f * scale_factor, 1.0f * scale_factor, -1.0f * scale_factor },	// Top-left-front vertex
		{ 1.0f * scale_factor, 1.0f * scale_factor, -1.0f * scale_factor },	// Top-right-front vertex
		{ -1.0f * scale_factor, -1.0f * scale_factor, 1.0f * scale_factor },	// Bottom-left-back vertex
		{ 1.0f * scale_factor, -1.0f * scale_factor, 1.0f * scale_factor },	// Bottom-right-back vertex
		{ -1.0f * scale_factor, 1.0f * scale_factor, 1.0f * scale_factor },	// Top-left-back vertex
		{ 1.0f * scale_factor, 1.0f * scale_factor, 1.0f * scale_factor },		// Top-right-back vertex
	};

	// Create the vertex buffer
	wrl::ComPtr<ID3D11Buffer> p_vertex_buffer;

	constexpr D3D11_BUFFER_DESC vertex_buffer_desc = {
		.ByteWidth = sizeof(vertices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u,
		.StructureByteStride = sizeof(vertex),
	};

	// ReSharper disable once CppVariableCanBeMadeConstexpr
	const D3D11_SUBRESOURCE_DATA vertex_subresource_data = {
		.pSysMem = vertices,
		.SysMemPitch = 0u,
		.SysMemSlicePitch = 0u
	};

	GFX_THROW_INFO(p_device_->CreateBuffer(&vertex_buffer_desc, &vertex_subresource_data, &p_vertex_buffer));

	// Bind the vertex buffer to the pipeline
	UINT stride = sizeof(vertex);
	UINT offset = 0u;
	p_device_context_->IASetVertexBuffers(0u, 1u, p_vertex_buffer.GetAddressOf(), &stride, &offset);

	// Create an index buffer structure
	const unsigned short indices[] =
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

		0, 2, 1,  2, 3, 1,  // White
		1, 3, 5,  3, 7, 5,  // Green
		2, 6, 3,  3, 6, 7,  // Red
		4, 5, 7,  4, 7, 6,  // Yellow
		0, 4, 2,  2, 4, 6,  // Blue
		0, 1, 4,  1, 5, 4,  // Orange
	};

	// Create the index buffer
	wrl::ComPtr<ID3D11Buffer> p_index_buffer;

	constexpr D3D11_BUFFER_DESC index_buffer_desc = {
		.ByteWidth = sizeof(indices),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u,
		.StructureByteStride = sizeof(unsigned short),
	};

	// ReSharper disable once CppVariableCanBeMadeConstexpr
	const D3D11_SUBRESOURCE_DATA index_subresource_data = {
		.pSysMem = indices,
		.SysMemPitch = 0u,
		.SysMemSlicePitch = 0u
	};

	GFX_THROW_INFO(p_device_->CreateBuffer(&index_buffer_desc, &index_subresource_data, &p_index_buffer));

	// Bind the index buffer to the pipeline
	stride = sizeof(unsigned short);
	offset = 0u;
	p_device_context_->IASetIndexBuffer(p_index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0u);

	// Create a constant buffer for a transformation matrix
	struct transform_constant_buffer { dx::XMMATRIX transform; };
	const transform_constant_buffer transform_constant_buffer = {
		{dx::XMMatrixTranspose(
			dx::XMMatrixRotationX(dx::XM_PIDIV4 + angle / 5) * 
			dx::XMMatrixRotationY(dx::XM_PIDIV4 + 2 * angle / 5) *
			dx::XMMatrixRotationZ(dx::XM_PIDIV4 + 3 * angle / 5) *
			dx::XMMatrixTranslation(x, y, z + 4.0f) *
			dx::XMMatrixPerspectiveLH(1.0f, aspect_ratio, 0.5f, 10.0f)
		)}
	};

	wrl::ComPtr<ID3D11Buffer> p_transform_constant_buffer;

	// ReSharper disable once CppVariableCanBeMadeConstexpr
	D3D11_BUFFER_DESC transform_constant_buffer_description = {
		.ByteWidth = sizeof(transform_constant_buffer),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0u,
		.StructureByteStride = 0u
	};

	D3D11_SUBRESOURCE_DATA transform_constant_buffer_subresource_data = {
		.pSysMem = &transform_constant_buffer,
		.SysMemPitch = 0u,
		.SysMemSlicePitch = 0u
	};

	GFX_THROW_INFO(p_device_->CreateBuffer(&transform_constant_buffer_description, &transform_constant_buffer_subresource_data, &p_transform_constant_buffer));

	// Bind the constant buffer to the pipeline
	p_device_context_->VSSetConstantBuffers(0u, 1u, p_transform_constant_buffer.GetAddressOf());

	struct face_color_constant_buffer
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};
	const face_color_constant_buffer face_color_constant_buffer =
	{
		{
			{ 1.0f, 1.0f, 1.0f, 1.0f }, // White
			{ 0.0f, 1.0f, 0.0f, 1.0f }, // Green
			{ 1.0f, 0.0f, 0.0f, 1.0f }, // Red
			{ 1.0f, 1.0f, 0.0f, 1.0f }, // Yellow
			{ 0.0f, 0.0f, 1.0f, 1.0f }, // Blue
			{ 1.0f, 0.5f, 0.0f, 1.0f }, // Orange
		}
	};

	wrl::ComPtr<ID3D11Buffer> p_face_color_constant_buffer;

	// ReSharper disable once CppVariableCanBeMadeConstexpr
	D3D11_BUFFER_DESC face_color_constant_buffer_description = {
		.ByteWidth = sizeof(face_color_constant_buffer),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = 0u,
		.MiscFlags = 0u,
		.StructureByteStride = 0u
	};

	D3D11_SUBRESOURCE_DATA face_color_constant_buffer_subresource_data = {
		.pSysMem = &face_color_constant_buffer,
		.SysMemPitch = 0u,
		.SysMemSlicePitch = 0u
	};

	GFX_THROW_INFO(p_device_->CreateBuffer(&face_color_constant_buffer_description, &face_color_constant_buffer_subresource_data, &p_face_color_constant_buffer));

	// Bind the constant buffer to the pipeline
	p_device_context_->PSSetConstantBuffers(0u, 1u, p_face_color_constant_buffer.GetAddressOf());

	{
		wrl::ComPtr<ID3DBlob> p_blob;

		// Create the pixel shader
		// uses blob
		{
			wrl::ComPtr<ID3D11PixelShader> p_pixel_shader;
			GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &p_blob));
			GFX_THROW_INFO(p_device_->CreatePixelShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), nullptr, &p_pixel_shader));

			// Bind the pixel shader
			p_device_context_->PSSetShader(p_pixel_shader.Get(), nullptr, 0u);
		}

		// Create the vertex shader
		// reuses blob
		{
			wrl::ComPtr<ID3D11VertexShader> p_vertex_shader;

			GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &p_blob));
			GFX_THROW_INFO(p_device_->CreateVertexShader(p_blob->GetBufferPointer(), p_blob->GetBufferSize(), nullptr, &p_vertex_shader));

			// Bind the vertex shader
			p_device_context_->VSSetShader(p_vertex_shader.Get(), nullptr, 0u);
		}

		// Input (vertex) layout (2D position only)
		// dependent on VS blob
		//  - Input Assembler
		{
			wrl::ComPtr<ID3D11InputLayout> p_input_layout;
			constexpr D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
				{
					.SemanticName = "POSITION",	// Input in the vertex shader
					.SemanticIndex = 0u,
					.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
					.InputSlot = 0u,
					.AlignedByteOffset = 0u,
					.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
					.InstanceDataStepRate = 0u
				},
			};
			GFX_THROW_INFO(p_device_->CreateInputLayout(
				input_element_desc,
				std::size(input_element_desc),
				p_blob->GetBufferPointer(),
				p_blob->GetBufferSize(),
				&p_input_layout));

			// Bind the input layout
			p_device_context_->IASetInputLayout(p_input_layout.Get());
		}
	}

	// Bind the render target
	//  - Output Merger
	p_device_context_->OMSetRenderTargets(1u, p_target_view_.GetAddressOf(), nullptr);

	// Set primitive topology to triangle list
	//  - Input Assembler
	p_device_context_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Configure the viewport
	//  - Rasterizer
	const D3D11_VIEWPORT viewport = {
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = width_,
		.Height = height_,
		.MinDepth = 0,
		.MaxDepth = 1
	};
	p_device_context_->RSSetViewports(1u, &viewport);

	GFX_THROW_INFO_ONLY(p_device_context_->DrawIndexed(std::size(indices), 0u, 0u));
}

// Graphics Exception
std::string graphics::graphics_exception::to_narrow(const wchar_t* s, const char fallback, const std::locale& loc) noexcept
{
	std::ostringstream out;
	while (*s != L'\0') {
		out << std::use_facet<std::ctype<wchar_t>>(loc).narrow(*s++, fallback);
	}
	return out.str();
}

graphics::hresult_exception::hresult_exception(const int line, const char* file, const HRESULT hresult, const std::vector<std::string>& info_messages) noexcept
	:
	graphics_exception(line, file),
	hresult_(hresult)
{
	// Join all the info messages into a single string
	for (const auto& message : info_messages)
	{
		info_message_ += message;
		info_message_.push_back('\n');
	}
}

const char* graphics::hresult_exception::what() const noexcept
{
	std::ostringstream out;
	out	<< "[Error Code] 0x" << std::hex << std::uppercase << get_error_code()
		<< std::dec << " (" << static_cast<unsigned long>(get_error_code()) << ")" << '\n'
		<< "[Error String] " << get_error_string() << '\n'
		<< "[Description] " << get_error_description() << '\n';
	if(!info_message_.empty())
	{
		out << "[Error Info]\n" << get_error_info() << '\n';
	}
	out << get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* graphics::hresult_exception::get_type() const noexcept
{
	return "Atum Graphics Exception";
}

HRESULT graphics::hresult_exception::get_error_code() const noexcept
{
	return hresult_;
}

std::string graphics::hresult_exception::get_error_string() const noexcept
{
#ifdef _UNICODE
	return to_narrow(DXGetErrorString(hresult_));
#else
	return DXGetErrorString(hresult_);
#endif
}

std::string graphics::hresult_exception::get_error_description() const noexcept
{
#ifdef _UNICODE
	WCHAR buffer[512];
	DXGetErrorDescription(hresult_, buffer, _countof(buffer));
	return to_narrow(buffer);
#else
	char buffer[512];
	DXGetErrorDescription(hresult_, buffer, _countof(buffer));
	return buffer;
#endif
}

std::string graphics::hresult_exception::get_error_info() const noexcept
{
	return info_message_;
}

graphics::info_exception::info_exception(int line, const char* file, const std::vector<std::string>& info_messages) noexcept
	:
	graphics_exception(line, file)
{
	// Join all the info messages into a single string
	for (const auto& message : info_messages)
	{
		info_message_ += message;
		info_message_.push_back('\n');
	}
}

const char* graphics::info_exception::what() const noexcept
{
	std::ostringstream out;
	out << get_type() << '\n';
	if (!info_message_.empty())
	{
		out << "[Error Info]\n" << get_error_info() << '\n';
	}
	out << get_origin_string();
	what_buffer_ = out.str();
	return what_buffer_.c_str();
}

const char* graphics::info_exception::get_type() const noexcept
{
	return "Atum Graphics Info Exception";
}

std::string graphics::info_exception::get_error_info() const noexcept
{
	return info_message_;
}

const char* graphics::device_removed_exception::get_type() const noexcept
{
	return "Atum Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
