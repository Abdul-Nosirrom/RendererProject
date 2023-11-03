#include "VertexShader.h"

#include "../../Errors/GraphicsErrors.h"

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
{
    SetDefines({});
    
    CreateVertexShader(gfx, path);
}

VertexShader::VertexShader(Graphics& gfx, const std::wstring& path, const std::vector<D3D_SHADER_MACRO>& defines)
{
    SetDefines(defines);
    
    CreateVertexShader(gfx, path);
}

void VertexShader::Bind(Graphics& gfx) noexcept
{
    GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob* VertexShader::GetBytecode() const noexcept
{
    return pBytecodeBlob.Get();
}

void VertexShader::CreateVertexShader(Graphics& gfx, const std::wstring& path)
{
    INFOMAN(gfx);

    CompileShader(path, "VSMain", "vs_5_0", &pBytecodeBlob);

    GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
        pBytecodeBlob->GetBufferPointer(),
        pBytecodeBlob->GetBufferSize(),
        nullptr,
        &pVertexShader));
}
