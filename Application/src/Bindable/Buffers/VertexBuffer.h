#pragma once
#include "Bindable/Bindable.h"
#include "Errors/GraphicsErrors.h"

struct Vertex
{
    Math::XMFLOAT3 pos;
};

class VertexBuffer : public Bindable
{
public:
    template<class V>
    VertexBuffer(Graphics& gfx, const std::vector<V>& vertices)
        : m_Stride(sizeof(V))
    {
        {
            INFOMAN(gfx);

            D3D11_BUFFER_DESC vbd = {};
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vbd.Usage = D3D11_USAGE_DEFAULT;
            vbd.CPUAccessFlags = 0u;
            vbd.MiscFlags = 0u;
            vbd.ByteWidth = UINT(m_Stride * vertices.size());
            vbd.StructureByteStride = m_Stride;

            D3D11_SUBRESOURCE_DATA vsd = {};
            vsd.pSysMem = vertices.data();

            GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &vsd, &pVertexBuffer));
        }
    }
    void Bind(Graphics& gfx) noexcept override;
protected:
    UINT m_Stride;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};
