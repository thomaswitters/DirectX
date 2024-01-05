#include "pch.h"
#include "Renderer.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		m_pEffect = new Effect(m_pDevice, L"Resources/PosCol3D.fx");

		vertices = {
		{{-3, 3, -2}, {1.f,1.f,1.f}, {0.f, 0.f}},
		{{0, 3, -2}, {1.f,1.f,1.f}, {0.5f, 0.f}},
		{{3, 3, -2}, {1.f,1.f,1.f}, {1.f, 0.f}},
		{{-3, 0, -2}, {1.f,1.f,1.f}, {0.f, 0.5f}},
		{{0, 0, -2}, {1.f,1.f,1.f}, {0.5f, 0.5f}},
		{{3, 0, -2}, {1.f,1.f,1.f}, {1.f, 0.5f}},
		{{-3, -3, -2}, {1.f,1.f,1.f}, {0.f, 1.f}},
		{{0, -3, -2}, {1.f,1.f,1.f}, {0.5f, 1.f}},
		{{3, -3, -2}, {1.f,1.f,1.f}, {1.f, 1.f}}
		};

		std::vector<uint32_t> indices{ 3, 0, 1,	  1, 4, 3,	 4, 1, 2,
									   2, 5, 4,	  6, 3, 4,	 4, 7, 6,
									   7, 4, 5,	  5, 8, 7 };
		m_pMesh = new Mesh(m_pEffect, m_pDevice, vertices, indices);

		m_pCamera = new Camera(Vector3(0.f, 0.f, -50.f), 45);

		textureUvGrid = Texture::LoadFromFile("Resources/uv_grid_2.png");
		if (!textureUvGrid)
		{
			std::cout << "Failed to load texture!\n";
			// Handle error (return, throw exception, etc.)
		}
		textureUvGrid->CreateDirectXTexture(m_pDevice); 
		//XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);
	}

	Renderer::~Renderer()
	{
		if (m_pMesh) 
		{
			delete m_pMesh;
		}
		if (m_pEffect)
		{
			delete m_pEffect;
		}
		if (m_pCamera)
		{
			delete m_pCamera;
		}
		if (textureUvGrid)
		{
			delete textureUvGrid;
		}
		if (m_pDevice)
		{
			m_pDevice->Release();
		}
		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}
		if (m_pSwapChain)
		{
			m_pSwapChain->Release();
		}
		if (m_pDepthStencilBuffer)
		{
			m_pDepthStencilBuffer->Release();
		}
		if (m_pDepthStancilView)
		{
			m_pDepthStancilView->Release();
		}
		if (m_pRenderTargetBuffer)
		{
			m_pRenderTargetBuffer->Release();
		}
		if (m_pRenderTargetView)
		{
			m_pRenderTargetView->Release();
		}
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);
	}


	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		//Clear RTV & DSV
		constexpr float color[4] = { 0.f, 0.f, 0.3f, 1.f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStancilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		Matrix scaleTransform = Matrix::CreateScale(1.f, 1.f, 1.f);
		Matrix rotationTransform = Matrix::CreateRotationY(0);

		for (const auto& vertex : vertices)
		{
			Matrix translationTransform = Matrix::CreateTranslation(vertex.position);
			Matrix worldMatrix = scaleTransform * translationTransform * rotationTransform;
			Matrix WorldViewProjectionMatrix = worldMatrix * m_pCamera->viewMatrix * m_pCamera->projectionMatrix;
			
			m_pEffect->SetWorldViewProjection(WorldViewProjectionMatrix);
			m_pEffect->SetDiffuseMap(textureUvGrid);
			m_pMesh->Render(m_pEffect, m_pDeviceContext);
		}
		
		

		m_pSwapChain->Present(0, 0);

	}

	HRESULT Renderer::InitializeDirectX()
	{	
		//Create Divice and DiviceContext
		//====
		D3D_FEATURE_LEVEL featurelevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featurelevel, 1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
			return result;

		//Create DXGI Factory
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
			return result;

		//Create Swapchain
		//====
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; 
		swapChainDesc.Flags = 0;

		//Get the handle (HWND) from the SDL backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_GetVersion(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//Create SwapChain
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
			return result;

		//Create DepthStencil (DS) & DepthStencilView (DSV)
		//Resource
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		//View
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
			return result;

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStancilView);
		if (FAILED(result))
			return result;

		//Create RenderTarget (RT) & RenderTargetView (RTV)
		//=====

		//Resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
			return result;
		//View
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
			return result;

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStancilView);

		// Set Viewport
		//=====
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;

		m_pDeviceContext->RSSetViewports(1, &viewport);

		if (pDxgiFactory)
		{
			pDxgiFactory->Release();
		}
		
		return S_OK;
	}
}
