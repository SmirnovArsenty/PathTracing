#include "render.h"
#include "primitive.h"
#include "octo_tree.h"
#include "anim.h"

using namespace pt;

renderer::renderer( HWND hWnd, unsigned int W, unsigned int H ) : m_camera(W, H)
{
	// Direct3D factory creation
	IDXGIFactory1 *Factory;
	if (CreateDXGIFactory1(IID_IDXGIFactory1, (void **)&Factory) != S_OK)
		throw std::exception("Error DXGI factory creation");

	// Select best (by video memory) graphics adapter
	IDXGIAdapter1 *Adapter, *BestAdapter = nullptr;
	unsigned int BestMemory = 0;
	for (int i = 0; Factory->EnumAdapters1(i, &Adapter) == S_OK; i++)
	{
		DXGI_ADAPTER_DESC1 desc = {0};
		Adapter->GetDesc1(&desc);

		if (BestAdapter == nullptr || desc.DedicatedVideoMemory >= BestMemory)
		{
			BestMemory = desc.DedicatedVideoMemory;
			if (BestAdapter != nullptr)
				BestAdapter->Release();
			BestAdapter = Adapter;
		}
		else
			Adapter->Release();
	}

	/* Set debug flag */
	unsigned int CreateDeviceFlags = 0;
#ifndef NDEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Feature levels
	D3D_FEATURE_LEVEL
		ResLevel,
		Level[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};

	// Direct3D device creation (for nullptr adapter -> D3D_DRIVER_TYPE_HARDWARE)
	if (D3D11CreateDevice(BestAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr,
			CreateDeviceFlags, Level, sizeof(Level) / sizeof(Level[0]),
			D3D11_SDK_VERSION, &m_device, &ResLevel, &m_context) != S_OK)
		throw std::exception("Error D3D11 device and context creation");
	BestAdapter->Release();

	// Adjust factory: disable ALT+ENTER fullscreen mode
	Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

	// Swap chain creation
	RECT rc;
	GetClientRect(hWnd, &rc);

	DXGI_SWAP_CHAIN_DESC SCDesc = {0};
	SCDesc.Flags = 0;
	SCDesc.BufferCount = 1;
	SCDesc.BufferDesc.Width= rc.right;
	SCDesc.BufferDesc.Height = rc.bottom;
	SCDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SCDesc.BufferDesc.RefreshRate.Numerator = 59;
	SCDesc.BufferDesc.RefreshRate.Denominator = 1;
	SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SCDesc.Windowed = TRUE;
	SCDesc.OutputWindow = hWnd;
	SCDesc.SampleDesc.Count = 1;
	SCDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (Factory->CreateSwapChain(m_device, &SCDesc, &m_swapChain) != S_OK)
		throw std::exception("Error DXGI swap chain creation");

	// Render target creation

	// Get back buffer texture
	ID3D11Texture2D *BackBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&BackBuffer);

	// Create render target
	m_device->CreateRenderTargetView(BackBuffer, nullptr, &m_renderTargetView);
	BackBuffer->Release();

	// Attach depth stencil to context (output merger)
	m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	// Set viewport (through rasterizer)
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)SCDesc.BufferDesc.Width;
	viewport.Height = (FLOAT)SCDesc.BufferDesc.Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &viewport);

	// Set rasterizer state
	D3D11_RASTERIZER_DESC RasterizeDescr;
	ID3D11RasterizerState *NewRSState;
	RasterizeDescr.CullMode = D3D11_CULL_NONE;
	RasterizeDescr.FillMode = D3D11_FILL_SOLID;
	// RasterizeDescr.FillMode = D3D11_FILL_WIREFRAME;
	RasterizeDescr.FrontCounterClockwise = FALSE;
	RasterizeDescr.DepthBias = 0;
	RasterizeDescr.DepthBiasClamp = 0.0f;
	RasterizeDescr.SlopeScaledDepthBias = 1.0f;
	RasterizeDescr.DepthClipEnable = TRUE;
	RasterizeDescr.ScissorEnable = FALSE;
	RasterizeDescr.MultisampleEnable = FALSE;
	RasterizeDescr.AntialiasedLineEnable = FALSE;

	m_device->CreateRasterizerState(&RasterizeDescr, &NewRSState);
	m_context->RSSetState(NewRSState);
	NewRSState->Release();
	Factory->Release();

	// load shaders
	shader shd(this, "pt");
	shd.apply();

	// load primitives
	vec Min, Max;

	primitive("resources\\models\\sphere.obj", &m_triangles, &Min, &Max, false, matr::scale(10) * matr::translate({0, 5, 0}), false, true);
	//float offset = 5;
	//for (int i = 0; i < 5; i++)
	//	for (int j = 0; j < 5; j++)
	//		primitive("resources\\models\\sphere.obj", &m_triangles, &Min, &Max, false, matr::scale(0.3) * matr::translate(vec((i - 2) * offset, 0, (j - 2) * offset)), false, false);

	if (m_triangles.size() != 0)
	{
		octo_tree *Tree = new octo_tree(octo_tree::box(Min, Max));
		for (auto t : m_triangles)
			Tree->Insert(t);

		std::vector<octo_tree::octo> O;
		std::vector<triangle> T;

		Tree->createStructuredBuffer(O, T);

		auto
			sizeoftriangle = sizeof(triangle),
			sizeofvertex = sizeof(vertex),
			sizeofvec = sizeof(vec),
			sizeofmaterial = sizeof(material);
		/* push triangles to shader */
		{
			D3D11_BUFFER_DESC Buf_descr;
			Buf_descr.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Buf_descr.ByteWidth = T.size() * sizeof(triangle);
			Buf_descr.CPUAccessFlags = 0;
			Buf_descr.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			Buf_descr.StructureByteStride = sizeof(triangle);
			Buf_descr.Usage = D3D11_USAGE_IMMUTABLE;
			
			D3D11_SUBRESOURCE_DATA SSData;
			SSData.pSysMem = T.data();
			SSData.SysMemPitch = 0;
			SSData.SysMemSlicePitch = 0;
			
			HRESULT res = m_device->CreateBuffer(&Buf_descr, &SSData, &m_triangleBuffer);
			if (res != S_OK)
				throw std::exception("Triangle buffer creation failed");
		}

		/* push octo vector to shader */
		{
			D3D11_BUFFER_DESC Buf_descr;
			Buf_descr.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			Buf_descr.ByteWidth = O.size() * sizeof(octo_tree::octo);
			Buf_descr.CPUAccessFlags = 0;
			Buf_descr.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			Buf_descr.StructureByteStride = sizeof(octo_tree::octo);
			Buf_descr.Usage = D3D11_USAGE_IMMUTABLE;
			
			D3D11_SUBRESOURCE_DATA SSData;
			SSData.pSysMem = O.data();
			SSData.SysMemPitch = 0;
			SSData.SysMemSlicePitch = 0;
			
			HRESULT res = m_device->CreateBuffer(&Buf_descr, &SSData, &m_octoTreeBuffer);
			if (res != S_OK)
				throw std::exception("Octo tree buffer creation failed");
		}

		/* set triangle vector to shader */
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SR_descr;
			SR_descr.BufferEx.FirstElement = 0;
			SR_descr.BufferEx.NumElements = T.size();
			SR_descr.BufferEx.Flags = 0;
			SR_descr.Format = DXGI_FORMAT_UNKNOWN;
			SR_descr.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			HRESULT res = m_device->CreateShaderResourceView(m_triangleBuffer, &SR_descr, &m_shaderRVTree);
			if (res != S_OK)
				throw std::exception("Shader resource view for triangle buffer creation failed");
			
			m_context->PSSetShaderResources(0, 1, &m_shaderRVTree);
		}

		/* set octo vector to shader */
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SR_descr;
			SR_descr.BufferEx.FirstElement = 0;
			SR_descr.BufferEx.NumElements = O.size();
			SR_descr.BufferEx.Flags = 0;
			SR_descr.Format = DXGI_FORMAT_UNKNOWN;
			SR_descr.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			
			HRESULT res = m_device->CreateShaderResourceView(m_octoTreeBuffer, &SR_descr, &m_shaderRVOcto);
			if (res != S_OK)
				throw std::exception("Shader resource view for octo tree buffer creation failed");
			
			m_context->PSSetShaderResources(1, 1, &m_shaderRVOcto);
		}
	}

	D3D11_BUFFER_DESC CBuf_descr;

	CBuf_descr.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBuf_descr.ByteWidth = 80;
	CBuf_descr.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBuf_descr.MiscFlags = 0;
	CBuf_descr.StructureByteStride = 0;
	CBuf_descr.Usage = D3D11_USAGE_DYNAMIC;

	m_device->CreateBuffer(&CBuf_descr, nullptr, &m_addData);
	m_context->PSSetConstantBuffers(0, 1, &m_addData);

	FLOAT ClearColor[] = {0.3f, 0.5f, 0.7f, 1.0f};

	m_context->ClearRenderTargetView(m_renderTargetView, ClearColor);
	m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

pt::renderer::~renderer()
{
	m_triangleBuffer->Release();
	m_octoTreeBuffer->Release();
	m_shaderRVTree->Release();
	m_shaderRVOcto->Release();
	m_addData->Release();

	m_device->Release();
	m_renderTargetView->Release();
	m_swapChain->Release();
	m_context->Release();
}

void pt::renderer::makeFrame()
{
	float data[] = {
		(float)anim::instance()->globalTime(),
		(float)m_camera.location().c_x(), (float)m_camera.location().c_y(), (float)m_camera.location().c_z(),
		(float)m_camera.width(),
		(float)m_camera.direction().c_x(), (float)m_camera.direction().c_y(), (float)m_camera.direction().c_z(),
		(float)m_camera.height(),
		(float)m_camera.right().c_x(), (float)m_camera.right().c_y(), (float)m_camera.right().c_z(),
		(float)m_camera.projectW(),
		(float)m_camera.up().c_x(), (float)m_camera.up().c_y(), (float)m_camera.up().c_z(),
		(float)m_camera.projectH(),
		(float)m_camera.projectDistance(),
		(float)m_triangles.size(),
		2.39f /* Reserved */};

	D3D11_MAPPED_SUBRESOURCE MSS;
	m_context->Map(m_addData, 0, D3D11_MAP_WRITE_DISCARD, 0, &MSS);
	memcpy(MSS.pData, data, 80);
	m_context->Unmap(m_addData, 0);

	m_context->Draw(3, 0);

	m_swapChain->Present(0, 0);
}
