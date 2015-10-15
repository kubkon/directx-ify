#include "pch.h"
#include "Engine.h"

using namespace MAPS;

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

	// initialise scene
	scene_ = Scene::CreateScene(device_.Get(), devContext_.Get());
	scene_->LoadMeshFromSTLFile(L"torus.stl");

	// initialise camera
	camera_ = Camera::CreateCamera();
	camera_->SetLens(
		XMConvertToRadians(45.0f),
		window->Bounds.Width / window->Bounds.Height,
		1.0f,
		1000.0f);
	camera_->UpdateViewMatrix();
}

void Engine::Update()
{
	// update camera's view matrix
	camera_->UpdateViewMatrix();
}

void Engine::Render()
{
	devContext_->OMSetRenderTargets(1, renderTarget_.GetAddressOf(), nullptr);

	// clear the back buffer to deep blue colour
	float colour[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devContext_->ClearRenderTargetView(renderTarget_.Get(), colour);

	// draw...
	scene_->Draw(camera_->View(), camera_->Proj());

	// switch the buffers
	ThrowIfFailed(swapChain_->Present(1, 0));
}


void Engine::MoveCamera(MoveDirection direction)
{
	if (direction == MoveDirection::Forward)
	{
		camera_->Walk(MOVE_DISTANCE);
	}
	else if (direction == MoveDirection::Backward)
	{
		camera_->Walk(MOVE_DISTANCE * -1.0f);
	}
	else if (direction == MoveDirection::Left)
	{
		camera_->Strafe(MOVE_DISTANCE * -1.0f);
	}
	else // direction == MoveDirection::Right
	{
		camera_->Strafe(MOVE_DISTANCE);
	}
}

void Engine::RotateCamera(float dx, float dy)
{
	camera_->Pitch(XMConvertToRadians(ROTATION_DISTANCE * dy));
	camera_->RotateY(XMConvertToRadians(ROTATION_DISTANCE * dx));
}
