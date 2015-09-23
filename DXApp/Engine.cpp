#include "pch.h"
#include "Engine.h"
#include "DXHelper.h"

using DXHelper::ThrowIfFailed;

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
	ThrowIfFailed(hr);
	ThrowIfFailed(device.As(&device_));
	ThrowIfFailed(devContext.As(&devContext_));

	ComPtr<IDXGIDevice1> dxgiDevice;
	ThrowIfFailed(device_.As(&dxgiDevice));
	ComPtr<IDXGIAdapter> dxgiAdapter;
	ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));
	ComPtr<IDXGIFactory2> dxgiFactory;
	ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory));

	DXGI_SWAP_CHAIN_DESC1 scd = { 0 };
	scd.BufferCount = 2;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	scd.SampleDesc.Count = 1;

	CoreWindow^ window = CoreWindow::GetForCurrentThread();
	ThrowIfFailed(dxgiFactory->CreateSwapChainForCoreWindow(
		device_.Get(),
		reinterpret_cast<IUnknown*>(window),
		&scd,
		nullptr,
		&swapChain_));
}

void Engine::Update()
{

}

void Engine::Render()
{
	ThrowIfFailed(swapChain_->Present(1, 0));
}
