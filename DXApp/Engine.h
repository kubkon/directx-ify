#pragma once
#include "pch.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;

class Engine
{
private:
	ComPtr<ID3D11Device1> m_device;
	ComPtr<ID3D11DeviceContext1> m_devContext;

public:
	void Initialize();
	void Update();
	void Render();
};
