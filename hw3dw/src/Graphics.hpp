#pragma once
#include "AtumException.hpp"
#include "DxgiInfoManager.hpp"

#include <d3d11.h>
#include <DirectXMath.h>
#include <locale>

struct ImVec4;

class Graphics
{
	friend class Bindable;
public:
	class GraphicsException : public AtumException
	{
		using AtumException::AtumException;
	protected:
		static std::string toNarrow(const wchar_t* s, char fallback = '?', const std::locale& loc = std::locale()) noexcept;
	};
	class HResultException : public GraphicsException
	{
	public:
		HResultException(int line, const char* file, HRESULT hresult, const std::vector<std::string>& infoMessages = {}) noexcept;
		const char* what() const noexcept override;
		const char* getType() const noexcept override;
		HRESULT getErrorCode() const noexcept;
		std::string getErrorString() const noexcept;
		std::string getErrorDescription() const noexcept;
		std::string getErrorInfo() const noexcept;
	private:
		HRESULT hresult_;
		std::string infoMessage_;
	};
	class InfoException final : public GraphicsException
	{
	public:
		InfoException(int line, const char* file, const std::vector<std::string>& infoMessages = {}) noexcept;
		const char* what() const noexcept override;
		const char* getType() const noexcept override;
		std::string getErrorInfo() const noexcept;
	private:
		std::string infoMessage_;
	};
	class DeviceRemovedException final : public HResultException
	{
		using HResultException::HResultException;
	public:
		const char* getType() const noexcept override;
	};
public:
	explicit Graphics(HWND parent, int width, int height);

	Graphics() = delete;
	~Graphics() = default;
	Graphics& operator=(const Graphics&) = delete;
	Graphics(const Graphics&&) = delete;
	Graphics& operator=(const Graphics&&) = delete;

	LRESULT handleMsg(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	void createRenderTarget();
	bool beginFrame(unsigned int targetWidth, unsigned int targetHeight);
	void endFrame();
	void clearBuffer(const ImVec4& clearColor) const;
	void clearBuffer(float red, float green, float blue, float alpha = 1.0) const;
	void drawIndexed(UINT count) noexcept(!IS_DEBUG);
	void setProjection(DirectX::FXMMATRIX& projection) noexcept;
	DirectX::XMMATRIX getProjection() const noexcept;
	static void shutdown();
private:
	HWND parent_;
	float width_, height_;
	bool swapChainOccluded_{};
#if (IS_DEBUG)
	DxgiInfoManager infoManager_;
#endif
	DirectX::XMMATRIX projection_;
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
};
