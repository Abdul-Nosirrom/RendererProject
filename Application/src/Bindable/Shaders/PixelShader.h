#pragma once
#include "Shader.h"

class PixelShader : public Shader
{
public:
    PixelShader(Graphics& gfx, const std::wstring& path);
    PixelShader(Graphics& gfx, const std::wstring& path, const std::vector<D3D_SHADER_MACRO>& defines);
    void Bind(Graphics& gfx) noexcept override;
protected:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

    void CreatePixelShader(Graphics& gfx, const std::wstring& path);
};
