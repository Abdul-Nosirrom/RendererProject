#pragma once
#include "RomanceWin.h" // Include first for all our switch cases since d3d11 also includes Windows.h
#include <d3d11.h>

/// Rundown of the various parts of D3D11
/// - DEVICE:   Must create a device, acts as an interface between the application and the graphics hardware. We use Device
///             whenever we want to allocate resources like a texture, buffer, shader, etc...
/// - CONTEXT:  We use a context to issue draw commands
class Graphics
{
public:
    Graphics(HWND hWnd);
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) const = delete;
    ~Graphics();

    /// @brief  Presents the back buffer to the front buffer (so to the screen)
    void SwapBuffer();

    /// @brief  Clears our RTV with the specified color
    void ClearBuffer(float r, float g, float b) noexcept;
    
private:
    ID3D11Device* pDevice;
    IDXGISwapChain* pSwapChain;
    ID3D11DeviceContext* pContext;
    ID3D11RenderTargetView* pTarget;
};
