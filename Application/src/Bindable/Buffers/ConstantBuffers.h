#pragma once

#include "Bindable/Bindable.h"
#include "Errors/GraphicsErrors.h"

template<typename C>
class ConstantBuffer : public Bindable
{
public:
    ConstantBuffer(Graphics& gfx, const C& cData)
    {
        INFOMAN(gfx);
    
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(cData);
        cbd.StructureByteStride = 0u;

        D3D11_SUBRESOURCE_DATA csd = {};
        csd.pSysMem = cData;

        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pCBuffer));
    }
    ConstantBuffer(Graphics& gfx)
    {
        INFOMAN(gfx);
    
        D3D11_BUFFER_DESC cbd;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(C);
        cbd.StructureByteStride = 0u;

        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pCBuffer));
    }
    void Update(Graphics& gfx, const C& cData)
    {
        INFOMAN(gfx);
    
        // Resource already sent to the GPU, we just need to modify it
        D3D11_MAPPED_SUBRESOURCE msd;
        
        GFX_THROW_INFO(GetContext(gfx)->Map(pCBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &msd));

        // We got the cbuffer data from the gpu, update its data
        memcpy(msd.pData, &cData, sizeof(cData));

        GetContext(gfx)->Unmap(pCBuffer.Get(), 0u);
    }

protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
    using ConstantBuffer<C>::pCBuffer;
    using Bindable::GetContext;
public:
    using ConstantBuffer<C>::ConstantBuffer;
    void Bind(Graphics& gfx) noexcept override
    {
        Bindable::GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pCBuffer.GetAddressOf());
    }
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
    using ConstantBuffer<C>::pCBuffer;
    using Bindable::GetContext;
public:
    using ConstantBuffer<C>::ConstantBuffer;
    void Bind(Graphics& gfx) noexcept override
    {
        Bindable::GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pCBuffer.GetAddressOf());
    }
};



