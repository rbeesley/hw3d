#pragma once
#include "AtumException.h"
#include "DxgiInfoManager.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <locale>

struct ImVec4;

class graphics
{
	friend class bindable;
public:
	class graphics_exception : public atum_exception
	{
		using atum_exception::atum_exception;
	protected:
		static std::string to_narrow(const wchar_t* s, char fallback = '?', const std::locale& loc = std::locale()) noexcept;
	};
	class hresult_exception : public graphics_exception
	{
	public:
		hresult_exception(int line, const char* file, HRESULT hresult, const std::vector<std::string>& info_messages = {}) noexcept;
		const char* what() const noexcept override;
		const char* get_type() const noexcept override;
		HRESULT get_error_code() const noexcept;
		std::string get_error_string() const noexcept;
		std::string get_error_description() const noexcept;
		std::string get_error_info() const noexcept;
	private:
		HRESULT hresult_;
		std::string info_message_;
	};
	class info_exception final : public graphics_exception
	{
	public:
		info_exception(int line, const char* file, const std::vector<std::string>& info_messages = {}) noexcept;
		const char* what() const noexcept override;
		const char* get_type() const noexcept override;
		std::string get_error_info() const noexcept;
	private:
		std::string info_message_;
	};
	class device_removed_exception final : public hresult_exception
	{
		using hresult_exception::hresult_exception;
	public:
		const char* get_type() const noexcept override;
	};
public:
	explicit graphics(HWND parent, int width, int height);

	graphics() = delete;
	~graphics() = default;
	graphics(const graphics&) = delete;
	graphics& operator=(const graphics&) = delete;
	graphics(const graphics&&) = delete;
	graphics& operator=(const graphics&&) = delete;

	void create_render_target();
	bool begin_frame(unsigned int target_width, unsigned int target_height);
	void end_frame();
	void clear_buffer(const ImVec4& clear_color) const;
	void clear_buffer(float red, float green, float blue, float alpha = 1.0) const;
	void draw_indexed(UINT count) noexcept(!IS_DEBUG);
	void set_projection(DirectX::FXMMATRIX& projection) noexcept;
	DirectX::XMMATRIX get_projection() const noexcept;
	static void shutdown();
private:
	HWND parent_;
	float width_, height_;
	bool swap_chain_occluded_{};
#if (IS_DEBUG)
	dxgi_info_manager info_manager_;
#endif
	DirectX::XMMATRIX projection_;
	Microsoft::WRL::ComPtr<ID3D11Device> p_device_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> p_swap_chain_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> p_device_context_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> p_render_target_view_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> p_depth_stencil_view_;
};
