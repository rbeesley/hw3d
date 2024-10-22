#pragma once
#include "AtumException.h"
#include "DxgiInfoManager.h"

#include <d3d11.h>
#include <locale>
#include <vector>
#include <wrl.h>

class graphics
{
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
	~graphics() = default;
	graphics(const graphics&) = delete;
	graphics& operator=(const graphics&) = delete;
	graphics(const graphics&&) = delete;
	graphics& operator=(const graphics&&) = delete;
	void end_frame();
	void clear_buffer(float red, float green, float blue) const;

	// Jam in experimental code to try and draw our first triangle
	void draw_test_triangle(float angle, float x, float y, float z);

private:
	HWND parent_;
	float width_, height_;
#if defined(DEBUG) || defined(_DEBUG)
	dxgi_info_manager info_manager_;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> p_device_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> p_swap_chain_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> p_context_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> p_target_view_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> p_depth_stencil_view_;
};
