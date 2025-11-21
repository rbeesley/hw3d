#pragma once
#include <DirectXMath.h>

class Camera {
public:
    explicit Camera(float fov, float aspectRatio, float nearZ, float farZ);

    void setFOV(float fov);
    void setPosition(DirectX::XMFLOAT3 pos);
    void setTarget(DirectX::XMFLOAT3 target);
    float getFOV() const;
    DirectX::XMFLOAT3 getPosition() const;
    DirectX::XMFLOAT3 getTarget() const;

    DirectX::XMMATRIX getView() const;
    DirectX::XMMATRIX getProjection() const;

private:
    void updateView();
    void updateProjection();

    float fov_;
    float aspectRatio_;
    float nearZ_;
    float farZ_;

	DirectX::XMFLOAT3 position_;
    DirectX::XMFLOAT3 target_;
    DirectX::XMFLOAT4 upDirection_;

	DirectX::XMMATRIX view_;
    DirectX::XMMATRIX projection_;
};
