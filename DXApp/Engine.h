#pragma once
#include "pch.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;

class Engine
{
private:
	ComPtr<ID3D11Device1> device_;
	ComPtr<ID3D11DeviceContext1> devContext_;
	ComPtr<IDXGISwapChain1> swapChain_;

public:
	void Initialize();
	void Update();
	void Render();
};
