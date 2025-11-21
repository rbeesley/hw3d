#pragma once

#include "AtumException.hpp"
#include "Camera.hpp"
#include "DxgiInfoManager.hpp"

#include <d3d11.h>
#include <DirectXMath.h>
#include <locale>
#include "backends/imgui_impl_dx11.h"

struct ImVec4;

class Graphics {
    friend class Bindable;

public:
    // -----------------------------
    // Lifecycle
    // -----------------------------
    explicit Graphics(HWND parent, int width, int height);
    Graphics() = delete;
    ~Graphics();

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(Graphics&&) = delete;

    // -----------------------------
    // ImGui Backend Wrapper
    // -----------------------------
    class ImGui {
    public:
        static void Init(ID3D11Device* device, ID3D11DeviceContext* context) {
            ImGui_ImplDX11_Init(device, context);
        }
        static void NewFrame() { ImGui_ImplDX11_NewFrame(); }
        static void Shutdown() { ImGui_ImplDX11_Shutdown(); }
    };

    // -----------------------------
    // Frame Management
    // -----------------------------
    bool beginFrame(unsigned int targetWidth, unsigned int targetHeight);
    void endFrame();
    void clearBuffer(const ImVec4& clearColor) const;
    void clearBuffer(float red, float green, float blue, float alpha = 1.0f) const;

    // -----------------------------
    // Rendering
    // -----------------------------
    void drawIndexed(UINT count) noexcept(!IS_DEBUG);

    // -----------------------------
    // Camera & Projection
    // -----------------------------
    void setCamera(std::unique_ptr<Camera> camera) noexcept;
    Camera* getCamera() const noexcept;
    void setProjection(DirectX::FXMMATRIX& projection) noexcept;
    DirectX::XMMATRIX getProjection() const noexcept;

    // -----------------------------
    // Message Handling
    // -----------------------------
    LRESULT handleMsg(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    // -----------------------------
    // Accessors
    // -----------------------------
    ID3D11Device* getDevice() const noexcept;
    ID3D11DeviceContext* getDeviceContext() const noexcept;

private:
    // -----------------------------
    // Internal Helpers
    // -----------------------------
    void createRenderTarget();

    // -----------------------------
    // Members
    // -----------------------------
    HWND parent_;
    float width_, height_;
    bool swapChainOccluded_{};
    std::unique_ptr<Camera> activeCamera_;
#if (IS_DEBUG)
    DxgiInfoManager infoManager_;
#endif
    DirectX::XMMATRIX projection_;
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;

public:
    // -----------------------------
    // Exception Types
    // -----------------------------
    class GraphicsException : public AtumException {
        using AtumException::AtumException;
    protected:
        static std::string toNarrow(const wchar_t* s, char fallback = '?', const std::locale& loc = std::locale()) noexcept;
    };

    class HResultException : public GraphicsException {
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

    class InfoException final : public GraphicsException {
    public:
        InfoException(int line, const char* file, const std::vector<std::string>& infoMessages = {}) noexcept;
        const char* what() const noexcept override;
        const char* getType() const noexcept override;
        std::string getErrorInfo() const noexcept;
    private:
        std::string infoMessage_;
    };

    class DeviceRemovedException final : public HResultException {
        using HResultException::HResultException;
    public:
        const char* getType() const noexcept override;
    };
};