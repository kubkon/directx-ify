#pragma once
#include "pch.h"
#include "Camera.h"
#include "Scene.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;

namespace MAPS
{
	struct VERTEX
	{
		XMFLOAT3 Position;
		XMFLOAT3 Colour;
	};

	class Engine
	{
	public:
		enum class MoveDirection
		{
			Forward, Backward,
			Left, Right
		};
		const float MOVE_DISTANCE = 0.5f;
		const float ROTATION_DISTANCE = 0.25f;

		void Initialize();
		void Update();
		void Render();

		void MoveCamera(MoveDirection);
		void RotateCamera(float, float);

	private:
		std::unique_ptr<Scene> scene_;
		std::unique_ptr<Camera> camera_;
		XMFLOAT4X4 world_;

		ComPtr<ID3D11Device1> device_;
		ComPtr<ID3D11DeviceContext1> devContext_;
		ComPtr<IDXGISwapChain1> swapChain_;
		ComPtr<ID3D11RenderTargetView> renderTarget_;
		ComPtr<ID3D11VertexShader> vertexShader_;
		ComPtr<ID3D11PixelShader> pixelShader_;
		ComPtr<ID3D11InputLayout> inputLayout_;
		ComPtr<ID3D11Buffer> constantBuffer_;
	};
};
