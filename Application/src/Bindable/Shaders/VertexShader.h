#pragma once
#include "Shader.h"

class VertexShader : public Shader
{
public:
    VertexShader(Graphics& gfx, const std::wstring& path);
    VertexShader(Graphics& gfx, const std::wstring& path, const std::vector<D3D_SHADER_MACRO>& defines);
    void Bind(Graphics& gfx) noexcept override;
    ID3DBlob* GetBytecode() const noexcept;
protected:
    Microsoft::WRL::ComPtr<ID3DBlob> pBytecodeBlob;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

    void CreateVertexShader(Graphics& gfx, const std::wstring& path);
};
