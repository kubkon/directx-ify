#include "pch.h"
#include "Camera.h"

using namespace MAPS;

Camera::Camera()
    : position_(0.0f, 0.0f, 0.0f),
      right_(1.0f, 0.0f, 0.0f),
      up_(0.0f, 1.0f, 0.0f),
      look_(0.0f, 0.0f, 1.0f)
{
    SetLens(0.25f * XM_PI, 1.0f, 1.0f, 1000.0f);
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
    fovY_ = fovY;
    aspect_ = aspect;
    nearZ_ = zn;
    farZ_ = zf;

    XMMATRIX proj = XMMatrixPerspectiveFovLH(fovY_, aspect_, nearZ_, farZ_);
    XMStoreFloat4x4(&proj_, proj);
}

void Camera::UpdateViewMatrix()
{
    XMVECTOR rightV = XMLoadFloat3(&right_);
    XMVECTOR upV = XMLoadFloat3(&up_);
    XMVECTOR lookV = XMLoadFloat3(&look_);
    XMVECTOR positionV = XMLoadFloat3(&position_);

    // orthonormalise the right, up and look vectors
    lookV = XMVector3Normalize(lookV);
    upV = XMVector3Normalize(XMVector3Cross(lookV, rightV));
    rightV = XMVector3Cross(upV, lookV);

    XMStoreFloat3(&right_, rightV);
    XMStoreFloat3(&look_, lookV);
    XMStoreFloat3(&up_, upV);

    // compute the view transformation matrix
    float x = -XMVectorGetX(XMVector3Dot(positionV, rightV));
    float y = -XMVectorGetX(XMVector3Dot(positionV, upV));
    float z = -XMVectorGetX(XMVector3Dot(positionV, lookV));

    view_(0, 0) = right_.x;
    view_(1, 0) = right_.y;
    view_(2, 0) = right_.z;
    view_(3, 0) = x;

    view_(0, 1) = up_.x;
    view_(1, 1) = up_.y;
    view_(2, 1) = up_.z;
    view_(3, 1) = y;

    view_(0, 2) = look_.x;
    view_(1, 2) = look_.y;
    view_(2, 2) = look_.z;
    view_(3, 2) = z;

    view_(0, 3) = 0.0f;
    view_(1, 3) = 0.0f;
    view_(2, 3) = 0.0f;
    view_(3, 3) = 1.0f;
}

XMMATRIX Camera::View() const
{
    return XMLoadFloat4x4(&view_);
}

XMMATRIX Camera::Proj() const
{
    return XMLoadFloat4x4(&proj_);
}

void Camera::Walk(float dist)
{
    XMVECTOR distV = XMVectorReplicate(dist);
    XMVECTOR lookV = XMLoadFloat3(&look_);
    XMVECTOR positionV = XMLoadFloat3(&position_);
    XMStoreFloat3(&position_, XMVectorMultiplyAdd(distV, lookV, positionV));
}

void Camera::Strafe(float dist)
{
    XMVECTOR distV = XMVectorReplicate(dist);
    XMVECTOR rightV = XMLoadFloat3(&right_);
    XMVECTOR positionV = XMLoadFloat3(&position_);
    XMStoreFloat3(&position_, XMVectorMultiplyAdd(distV, rightV, positionV));
}

void Camera::Pitch(float angle)
{
    XMMATRIX rotationM = XMMatrixRotationAxis(XMLoadFloat3(&right_), angle);
    XMStoreFloat3(&up_, XMVector3TransformNormal(XMLoadFloat3(&up_), rotationM));
    XMStoreFloat3(&look_, XMVector3TransformNormal(XMLoadFloat3(&look_), rotationM));
}

void Camera::RotateY(float angle)
{
    XMMATRIX rotationM = XMMatrixRotationY(angle);
    XMStoreFloat3(&right_, XMVector3TransformNormal(XMLoadFloat3(&right_), rotationM));
    XMStoreFloat3(&up_, XMVector3TransformNormal(XMLoadFloat3(&up_), rotationM));
    XMStoreFloat3(&look_, XMVector3TransformNormal(XMLoadFloat3(&look_), rotationM));
}
