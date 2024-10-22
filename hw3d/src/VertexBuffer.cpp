//struct vertex
//{
//	struct { float x; float y; float z; } pos;
//};
//
//// 4:3 aspect ratio correction
//const float aspect_ratio = height_ / width_;
//constexpr float scale_factor = 1.0f;
//
//// Create a vertex buffer structure
//const vertex vertices[] =
//{
//	//    6-------7
//	//   /|      /|
//	//  2-------3 |
//	//  | |     | |
//	//  | 4-----|-5
//	//  |/      |/
//	//  0-------1
//
//	{ -1.0f * scale_factor, -1.0f * scale_factor, -1.0f * scale_factor },	// Bottom-left-front vertex
//	{ 1.0f * scale_factor, -1.0f * scale_factor, -1.0f * scale_factor },	// Bottom-right-front vertex
//	{ -1.0f * scale_factor, 1.0f * scale_factor, -1.0f * scale_factor },	// Top-left-front vertex
//	{ 1.0f * scale_factor, 1.0f * scale_factor, -1.0f * scale_factor },	// Top-right-front vertex
//	{ -1.0f * scale_factor, -1.0f * scale_factor, 1.0f * scale_factor },	// Bottom-left-back vertex
//	{ 1.0f * scale_factor, -1.0f * scale_factor, 1.0f * scale_factor },	// Bottom-right-back vertex
//	{ -1.0f * scale_factor, 1.0f * scale_factor, 1.0f * scale_factor },	// Top-left-back vertex
//	{ 1.0f * scale_factor, 1.0f * scale_factor, 1.0f * scale_factor },		// Top-right-back vertex
//};
//
//wrl::ComPtr<ID3D11Buffer> p_vertex_buffer;
//
//constexpr D3D11_BUFFER_DESC vertex_buffer_desc = {
//	.ByteWidth = sizeof(vertices),
//	.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT,
//	.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
//	.CPUAccessFlags = 0u,
//	.MiscFlags = 0u,
//	.StructureByteStride = sizeof(vertex),
//};
//
//// ReSharper disable once CppVariableCanBeMadeConstexpr
//const D3D11_SUBRESOURCE_DATA vertex_subresource_data = {
//	.pSysMem = vertices,
//	.SysMemPitch = 0u,
//	.SysMemSlicePitch = 0u
//};
//
//GFX_THROW_INFO(p_device_->CreateBuffer(&vertex_buffer_desc, &vertex_subresource_data, &p_vertex_buffer));

// Bind the vertex buffer to the pipeline
//UINT stride = sizeof(vertex);
//UINT offset = 0u;
//p_context_->IASetVertexBuffers(0u, 1u, p_vertex_buffer.GetAddressOf(), &stride, &offset);

#include "VertexBuffer.h"

void vertex_buffer::bind(graphics& graphics) noexcept
{
	constexpr UINT offset = 0u;
	get_context(graphics)->IASetVertexBuffers(0u, 1u, p_vertex_buffer.GetAddressOf(), &stride, &offset);
}
