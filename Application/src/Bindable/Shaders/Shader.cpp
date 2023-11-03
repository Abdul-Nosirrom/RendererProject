#include "Shader.h"

#include <d3dcompiler.h>

void Shader::SetDefines(const std::vector<D3D_SHADER_MACRO>& defines) noexcept
{
    m_Defines = defines;
}

void Shader::CompileShader(const std::wstring& path, const char* entryPoint, const char* profile,
                           ID3DBlob** ppBlob)
{
    Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;
    
    UINT flags = 0u; //D3DCOMPILE_ENABLE_STRICTNESS;

    // Cant use throw macro, have to retrieve error msg from pErrorBlob
    HRESULT compHR = D3DCompileFromFile(
        path.c_str(),
        m_Defines.data(),
        nullptr,
        entryPoint,
        profile,
        flags, 0u, ppBlob, &pErrorBlob);

    if (compHR < 0)
    {
        auto err = (char*)pErrorBlob->GetBufferPointer();
        throw Graphics::HrException(__LINE__, __FILE__, compHR, {err});
    }
}
