#include "InputLayout.h"

#include "../../Errors/GraphicsErrors.h"

InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& ied, ID3DBlob* pVertexShaderBytecode)
{
    INFOMAN(gfx);
    GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
        ied.data(),
        UINT(ied.size()),
        pVertexShaderBytecode->GetBufferPointer(),
        pVertexShaderBytecode->GetBufferSize(),
        &pInputLayout));
}

void InputLayout::Bind(Graphics& gfx) noexcept
{
    GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}
