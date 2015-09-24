#include "pch.h"
#include "Engine.h"
#include "DXHelper.h"
#include <fstream>

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

	// initialise graphics and pipeline
	InitGraphics();
	InitPipeline();
}

void Engine::Update()
{

}

void Engine::Render()
{
	devContext_->OMSetRenderTargets(1, renderTarget_.GetAddressOf(), nullptr);

	// clear the back buffer to deep blue colour
	float colour[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devContext_->ClearRenderTargetView(renderTarget_.Get(), colour);

	// set the vertex buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devContext_->IASetVertexBuffers(
		0,
		1,
		vertexBuffer_.GetAddressOf(),
		&stride,
		&offset);

	// set the primitive topology
	devContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// draw...
	devContext_->Draw(ARRAYSIZE(triangles_), 0);

	// switch the buffers
	ThrowIfFailed(swapChain_->Present(1, 0));
}

void Engine::InitGraphics()
{
	VERTEX v0 = { { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
	VERTEX v1 = { { -0.45f, -0.5f, 0.0 }, { 0.0f, 1.0f, 0.0f } };
	VERTEX v2 = { { 0.45f, -0.5f, 0.0 }, { 0.0f, 0.0f, 1.0f } };
	VERTEX v3 = { { 0.45f, 0.5f, 0.0 }, { 0.0f, 0.0f, 0.0f } };

	triangles_[0] = v0;
	triangles_[1] = v1;
	triangles_[2] = v2;
	triangles_[3] = v0;
	triangles_[4] = v2;
	triangles_[5] = v3;

	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(triangles_);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { triangles_, 0, 0 };

	ThrowIfFailed(device_->CreateBuffer(&bd, &srd, &vertexBuffer_));
}

void Engine::InitPipeline()
{
	Array<byte>^ vsFile = LoadShaderFile("VertexShader.cso");
	Array<byte>^ psFile = LoadShaderFile("PixelShader.cso");

	ThrowIfFailed(device_->CreateVertexShader(
		vsFile->Data,
		vsFile->Length,
		nullptr,
		&vertexShader_));
	ThrowIfFailed(device_->CreatePixelShader(
		psFile->Data,
		psFile->Length,
		nullptr,
		&pixelShader_));

	devContext_->VSSetShader(vertexShader_.Get(), nullptr, 0);
	devContext_->PSSetShader(pixelShader_.Get(), nullptr, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ThrowIfFailed(device_->CreateInputLayout(
		ied,
		ARRAYSIZE(ied),
		vsFile->Data,
		vsFile->Length,
		&inputLayout_));

	devContext_->IASetInputLayout(inputLayout_.Get());
}

Array<byte>^ Engine::LoadShaderFile(std::string file)
{
	Array<byte>^ fileData = nullptr;

	// open file
	std::ifstream vertexFile(
		file,
		std::ios::in | std::ios::binary | std::ios::ate);

	if (vertexFile.is_open())
	{
		// get the length of the file
		int length = (int)vertexFile.tellg();

		// collect the file data
		fileData = ref new Array<byte>(length);
		vertexFile.seekg(0, std::ios::beg);
		vertexFile.read(reinterpret_cast<char*>(fileData->Data), length);
		vertexFile.close();
	}

	return fileData;
}
