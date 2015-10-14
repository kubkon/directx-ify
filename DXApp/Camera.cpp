#include "pch.h"
#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

std::unique_ptr<Camera> Camera::CreateCamera()
{
	std::unique_ptr<Camera> camera(new Camera());
	return camera;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{

	XMMATRIX proj = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
	XMStoreFloat4x4(&proj_, proj);
}

void Camera::UpdateViewMatrix()
{
	// compute the view transformation matrix
	XMVECTOR camPosition = XMVectorSet(1.5f, 1.5f, 10.0f, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(camPosition, camLookAt, camUp);
	XMStoreFloat4x4(&view_, view);
}

XMMATRIX Camera::View() const
{
	return XMLoadFloat4x4(&view_);
}

XMMATRIX Camera::Proj() const
{
	return XMLoadFloat4x4(&proj_);
}