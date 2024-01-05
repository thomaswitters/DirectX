#include "pch.h"
#include "Mesh.h"
#include <cassert>

namespace dae
{
	Mesh::Mesh(Effect* pEffect, ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices)
	{
		//Create Vertex Layout
		static constexpr uint32_t numElemets{ 2 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElemets]{};

		vertexDesc[0].SemanticName = "POSITION";
		vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset = 0;
		vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[1].SemanticName = "COLOR";
		vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[1].AlignedByteOffset = 12;
		vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		//vertexDesc[2].SemanticName = "TEXCOORD";
		//vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		//vertexDesc[2].AlignedByteOffset = 24;  // Assuming 4 floats for POSITION and 3 floats for COLOR
		//vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

		//Create Input Layout
		D3DX11_PASS_DESC passDesc{};
		pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);

		const HRESULT result = pDevice->CreateInputLayout(
			vertexDesc,
			numElemets,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout);

		if (FAILED(result))
			assert(false);

		// Create vertex buffer 
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = vertices.data();

		HRESULT Result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
		if (FAILED(Result))
			return;

		//Create index buffer
		m_NumIndices = static_cast<uint32_t>(indices.size());
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		initData.pSysMem = indices.data();

		Result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
		if (FAILED(Result))
			return;


	}

	void Mesh::Render(Effect* pEffect, ID3D11DeviceContext* pDeviceContext)
	{
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pDeviceContext->IASetInputLayout(m_pInputLayout);

		constexpr UINT stride = sizeof(Vertex_PosCol);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		D3DX11_TECHNIQUE_DESC techDesc{};
		pEffect->GetTechnique()->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			pEffect->GetTechnique()->GetPassByIndex(p)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}

	Mesh::~Mesh()
	{
		m_pVertexBuffer->Release();
		m_pIndexBuffer->Release();
		m_pInputLayout->Release();
	}
}


