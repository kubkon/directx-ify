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

	proj_ = XMMatrixPerspectiveFovLH(fovY, aspect, zn, zf);
}

void Camera::UpdateViewMatrix()
{
	// compute the view transformation matrix
	XMVECTOR camPosition = XMVectorSet(1.5f, 1.5f, 10.0f, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	view_ = XMMatrixLookAtLH(camPosition, camLookAt, camUp);
}

XMMATRIX Camera::View() const
{
	return view_;
}

XMMATRIX Camera::Proj() const
{
	return proj_;
}