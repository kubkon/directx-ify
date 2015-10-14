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

private:
	Camera();

	XMFLOAT4X4 view_;
	XMFLOAT4X4 proj_;
};