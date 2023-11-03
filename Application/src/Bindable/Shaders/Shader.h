#pragma once
#include "../Bindable.h"

class Shader : public Bindable
{
public:
    void SetDefines(const std::vector<D3D_SHADER_MACRO>& defines) noexcept;
    
protected:
    std::vector<D3D_SHADER_MACRO> m_Defines;
    void CompileShader(const std::wstring& path, const char* entryPoint, const char* profile, ID3DBlob** ppBlob);
};
