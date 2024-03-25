#pragma once
#include "Graphics.h"

class Bindable
{
public:
    virtual void Bind(Graphics& gfx) noexcept = 0;
    virtual ~Bindable() = default;
protected:
    /* Static accessors to graphics objects, Bindable is a friend class of graphics so we can only access it through here */
    static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
    static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
    static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept(!IS_DEBUG);
};
