#pragma once
#include "pch.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Platform;

typedef struct VERTEX
{
	float X, Y, Z;
} VERTEX;

class Engine
{
public:
	void Initialize();
	void Update();
	void Render();

private:
	void InitGraphics();
	void InitPipeline();
	Array<byte>^ LoadShaderFile(std::string);

	ComPtr<ID3D11Device1> device_;
	ComPtr<ID3D11DeviceContext1> devContext_;
	ComPtr<IDXGISwapChain1> swapChain_;
	ComPtr<ID3D11RenderTargetView> renderTarget_;
	ComPtr<ID3D11Buffer> vertexBuffer_;
	ComPtr<ID3D11VertexShader> vertexShader_;
	ComPtr<ID3D11PixelShader> pixelShader_;
	ComPtr<ID3D11InputLayout> inputLayout_;
};
