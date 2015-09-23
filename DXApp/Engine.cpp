#include "pch.h"
#include "Engine.h"
#include "DXHelper.h"

void Engine::Initialize()
{
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> devContext;

	auto hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&device,
		nullptr,
		&devContext
		);
	DXHelper::ThrowIfFailed(hr);
	hr = device.As(&device_);
	hr = devContext.As(&devContext_);
}

void Engine::Update()
{

}

void Engine::Render()
{

}
