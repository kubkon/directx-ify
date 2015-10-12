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

	// initialise time
	time_ = 0.0f;
}

void Engine::Update()
{
	time_ += 0.5f;
}

void Engine::Render()
{
	devContext_->OMSetRenderTargets(1, renderTarget_.GetAddressOf(), nullptr);

	// clear the back buffer to deep blue colour
	float colour[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devContext_->ClearRenderTargetView(renderTarget_.Get(), colour);

	// draw...
	FXMMATRIX world = GetWorldTransform();
	CXMMATRIX view = GetViewTransform();
	CXMMATRIX projection = GetProjectiveTransform();
	XMMATRIX transform = world * view * projection;
	devContext_->UpdateSubresource(constantBuffer_.Get(), 0, 0, &transform, 0, 0);
	shape_->Draw(
		world,
		view,
		projection,
		Colors::Gray,
		nullptr,
		true);

	// switch the buffers
	ThrowIfFailed(swapChain_->Present(1, 0));
}

void Engine::InitGraphics()
{
	shape_ = GeometricPrimitive::CreateSphere(devContext_.Get(), 1.0f);

	// here, we are going to read in the STL file
	model_ = Model::CreateFromSTL(device_.Get(), "liver.stl");

	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = 4 * 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;

	ThrowIfFailed(device_->CreateBuffer(&bd, nullptr, &constantBuffer_));
	devContext_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
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

XMMATRIX Engine::GetWorldTransform()
{
	XMMATRIX scale, rotate;
	float roll, pitch, yaw;

	roll = XMConvertToRadians(0.0f);
	pitch = XMConvertToRadians(time_);
	yaw = XMConvertToRadians(0.0f);

	scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	rotate = XMMatrixRotationRollPitchYaw(roll, pitch, yaw);

	return scale * rotate;
}

XMMATRIX Engine::GetViewTransform()
{
	XMVECTOR camPosition = XMVectorSet(1.5f, 0.5f, 1.5f, 0.0f);
	XMVECTOR camLookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return XMMatrixLookAtLH(camPosition, camLookAt, camUp);
}

XMMATRIX Engine::GetProjectiveTransform()
{
	float fovy, aspect, zn, zf;

	// set the field of view angle
	fovy = XMConvertToRadians(45.0f);
	
	// set the aspect ratio
	CoreWindow^ window = CoreWindow::GetForCurrentThread();
	aspect = window->Bounds.Width / window->Bounds.Height;

	// set the near view plane
	zn = 1.0f;

	// set the far view plane
	zf = 100.0f;

	return XMMatrixPerspectiveFovLH(fovy, aspect, zn, zf);
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
