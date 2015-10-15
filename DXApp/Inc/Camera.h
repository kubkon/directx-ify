#pragma once
#include "pch.h"

using namespace DirectX;

namespace MAPS
{
	class Camera
	{
	public:
		static std::unique_ptr<Camera> CreateCamera();
		virtual ~Camera();

		void SetLens(float, float, float, float);
		void UpdateViewMatrix();

		XMMATRIX View() const;
		XMMATRIX Proj() const;

		// walk camera a distance
		void Walk(float);

		// strafe camera a distance
		void Strafe(float);

		void Pitch(float);

		void RotateY(float);

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
};
