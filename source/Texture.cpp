#include "pch.h"
#include "Texture.h"

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ (uint32_t*)pSurface->pixels },
		m_pResource{},
		m_pSRV{}
	{
		m_pSurface = SDL_CreateRGBSurface(0, m_pSurface->w, m_pSurface->h, 32, 0, 0, 0, 0);


	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
		if (m_pSRV)
		{
			m_pSRV->Release();
			m_pSRV = nullptr;
		}

		if (m_pResource)
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (!loadedSurface)
		{
			std::cerr << "Failed to load image: " << SDL_GetError() << std::endl;
			return nullptr;
		}

		return new Texture(loadedSurface);
	}

	void Texture::CreateDirectXTexture(ID3D11Device* pDevice)
	{
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = m_pSurface->w;
		desc.Height = m_pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = m_pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pResource);
		if (FAILED(hr))
		{
			std::cerr << "Failed to create texture: " << hr << std::endl;
			//return ;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = pDevice->CreateShaderResourceView(m_pResource, &SRVDesc, &m_pSRV);
		if (FAILED(hr))
		{
			std::cerr << "Failed to create shader resource view: " << hr << std::endl;
			//return nullptr;
		}
	}
}