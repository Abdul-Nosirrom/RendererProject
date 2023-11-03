#include "PixelShader.h"

#include "../../Errors/GraphicsErrors.h"

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path)
{
    SetDefines({});
    
    CreatePixelShader(gfx, path);
}

PixelShader::PixelShader(Graphics& gfx, const std::wstring& path, const std::vector<D3D_SHADER_MACRO>& defines)
{
    SetDefines(defines);
    
    CreatePixelShader(gfx, path);
}

void PixelShader::Bind(Graphics& gfx) noexcept
{
    GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}

void PixelShader::CreatePixelShader(Graphics& gfx, const std::wstring& path)
{
    INFOMAN(gfx);

    Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;

    CompileShader(path, "PSMain", "ps_5_0", &pPSBlob);

    GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(
        pPSBlob->GetBufferPointer(),
        size_t(pPSBlob->GetBufferSize()),
        nullptr,
        &pPixelShader));
}
