#pragma once
#include "pch.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;

struct VERTEX
{
	XMFLOAT3 Position;
	XMFLOAT3 Colour;
};

class Engine
{
public:
	void Initialize();
	void Update();
	void Render();

private:
	void InitGraphics();
	void InitPipeline();
	XMMATRIX GetWorldTransform();
	XMMATRIX GetViewTransform();
	XMMATRIX GetProjectiveTransform();
	Array<byte>^ LoadShaderFile(std::string);

	VERTEX triangles_[6];
	float time_;

	ComPtr<ID3D11Device1> device_;
	ComPtr<ID3D11DeviceContext1> devContext_;
	ComPtr<IDXGISwapChain1> swapChain_;
	ComPtr<ID3D11RenderTargetView> renderTarget_;
	ComPtr<ID3D11Buffer> vertexBuffer_;
	ComPtr<ID3D11VertexShader> vertexShader_;
	ComPtr<ID3D11PixelShader> pixelShader_;
	ComPtr<ID3D11InputLayout> inputLayout_;
	ComPtr<ID3D11Buffer> constantBuffer_;
};
