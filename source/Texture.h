#pragma once

namespace dae
{
	class Texture
	{
	public:
		~Texture();

		static Texture* LoadFromFile(const std::string& path);
		ColorRGB Sample(const Vector2& uv) const;
		void CreateDirectXTexture(ID3D11Device* pDevice);

		ID3D11ShaderResourceView* GetSRV()
		{
			return m_pSRV;
		}
	private:
		Texture(SDL_Surface* pSurface);

		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
		ID3D11Texture2D* m_pResource;
		ID3D11ShaderResourceView* m_pSRV;
	};
}
