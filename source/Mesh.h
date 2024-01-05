#pragma once
#include "Math.h"
#include "vector"
#include "Effect.h"

namespace dae
{
	struct Vertex
	{
		Vector3 position;
		ColorRGB color{ colors::White };
		Vector2 uv{}; //W2
		Vector3 normal{}; //W4
		Vector3 tangent{}; //W4
		Vector3 viewDirection{}; //W4
	};

	struct Vertex_PosCol
	{
		Vector3 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		//Vector3 normal{};
		//Vector3 tangent{};
		//Vector3 viewDirection{};
	};


	/*struct Mesh
	{
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		std::vector<Vertex_Out> vertices_out{};
		Matrix worldMatrix{};
	};*/

	class Mesh
	{
	private:

		//std::vector<Vertex_PosCol> vertices{};
		//std::vector<uint32_t> indices{};

//		Effect* m_pEffect;
		ID3D11InputLayout* m_pInputLayout;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		uint32_t m_NumIndices{ 0 };

		ID3DX11EffectTechnique* m_pTechnique;

	public:
		Mesh(Effect* pEffect, ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices);

		void Render(Effect* pEffect, ID3D11DeviceContext* pDeviceContext);

		~Mesh();
	};
}



