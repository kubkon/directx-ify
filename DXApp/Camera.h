#pragma once
#include "pch.h"

using namespace DirectX;

__declspec(align(16))
class Camera
{
public:
	static std::unique_ptr<Camera> CreateCamera();

	virtual ~Camera();

	void* operator new(size_t i) { return _mm_malloc(i, 16); }
	void operator delete(void* p) { _mm_free(p); }

	void SetLens(float, float, float, float);
	void UpdateViewMatrix();

	XMMATRIX View() const;
	XMMATRIX Proj() const;

	// walk camera a distance
	void Walk(float);

private:
	Camera();

	// camera coordinate system with coordinates relative
	// to world space
	XMFLOAT3 position_; // view space origin
	XMFLOAT3 right_;    // view space x-axis
	XMFLOAT3 up_;		// view space y-axis
	XMFLOAT3 look_;		// view space z-axis

	// frustrum properties
	float nearZ_;
	float farZ_;
	float aspect_;
	float fovY_;

	// view and proj matrices
	XMFLOAT4X4 view_;
	XMFLOAT4X4 proj_;
};