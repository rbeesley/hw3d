#include "Camera.hpp"

Camera::Camera(float fov, float aspectRatio, float nearZ, float farZ)
    : fov_(fov), aspectRatio_(aspectRatio), nearZ_(nearZ), farZ_(farZ)
{
    position_ = { 0.0f, 0.0f, -10.0f };
    target_ = { 0.0f, 0.0f, 0.0f };
    upDirection_ = { 0.0f, 1.0f, 0.0f, 0.0f };

	updateProjection();
    updateView();
}

void Camera::setFOV(const float fov)
{
    fov_ = fov;
    updateProjection();
}

void Camera::setPosition(const DirectX::XMFLOAT3 pos)
{
	position_ = pos;
	updateView();
}

void Camera::setTarget(const DirectX::XMFLOAT3 target)
{
	target_ = target;
	updateView();
}

float Camera::getFOV() const
{
    return fov_;
}

DirectX::XMFLOAT3 Camera::getPosition() const
{
    return position_;
}

DirectX::XMFLOAT3 Camera::getTarget() const
{
    return target_;
}

DirectX::XMMATRIX Camera::getView() const
{
	return view_;
}

DirectX::XMMATRIX Camera::getProjection() const
{
	return projection_;
}

void Camera::updateView() {
    // Prevent the camera position from directly overlapping the target
    auto target = target_;

    if (position_.x == target_.x &&
        position_.y == target_.y &&
        position_.z == target_.z)
    {
        target.z -= 0.1f; // prevent zero direction
    }

    view_ = DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&position_),
        DirectX::XMLoadFloat3(&target),
        DirectX::XMVectorSet(upDirection_.x, upDirection_.y, upDirection_.z, upDirection_.w));
}

void Camera::updateProjection() {
    projection_ = DirectX::XMMatrixPerspectiveFovLH(fov_, aspectRatio_, nearZ_, farZ_);
}
