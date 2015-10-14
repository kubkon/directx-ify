#include "pch.h"
#include "Engine.h"
#include "DXHelper.h"

using DXHelper::ThrowIfFailed;

void Engine::Initialize()
{
	// initialise Direct3D
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> devContext;

	UINT createDeviceFlags = 0;

	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	auto hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
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

	// create the swap chain
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

	// set the render target
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));
	ThrowIfFailed(device_->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		&renderTarget_));

	// set the viewport
	D3D11_VIEWPORT viewport = { 0 };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = window->Bounds.Width;
	viewport.Height = window->Bounds.Height;
	devContext_->RSSetViewports(1, &viewport);

	// initialise graphics
	InitGraphics();

	// initialise camera
	camera_ = Camera::CreateCamera();
	camera_->SetLens(
		XMConvertToRadians(45.0f),
		window->Bounds.Width / window->Bounds.Height,
		1.0f,
		1000.0f);
	camera_->UpdateViewMatrix();

	// initialise world transform
	SetWorldMatrix(0.0f, 0.0f, 0.0f);

	// initialise time
	time_ = 0.0f;
}

void Engine::Update()
{
	time_ += 0.5f;
	SetWorldMatrix(0.0f, XMConvertToRadians(time_), 0.0f);
}

void Engine::Render()
{
	devContext_->OMSetRenderTargets(1, renderTarget_.GetAddressOf(), nullptr);

	// clear the back buffer to deep blue colour
	float colour[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devContext_->ClearRenderTargetView(renderTarget_.Get(), colour);

	// draw...
	CommonStates states(device_.Get());
	model_->Draw(
		devContext_.Get(),
		states,
		world_,
		camera_->View(),
		camera_->Proj(),
		true);

	// switch the buffers
	ThrowIfFailed(swapChain_->Present(1, 0));
}

void Engine::InitGraphics()
{
	// here, we are going to read in the STL file
	model_ = Model::CreateFromSTL(device_.Get(), "torus.stl");
}

void Engine::SetWorldMatrix(float roll, float pitch, float yaw)
{
	XMMATRIX scale, rotate;
	scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	rotate = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);

	// adjust for RH model
	world_ = scale * rotate;
	world_.r[2] *= -1.0f;
}
