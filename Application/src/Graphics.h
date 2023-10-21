#pragma once
#include "RomanceWin.h" // Include first for all our switch cases since d3d11 also includes Windows.h
#include <d3d11.h>
#include <wrl.h>
#include "DxgiInfoManager.h"

#include "RomanceException.h"

/// Rundown of the various parts of D3D11
/// - DEVICE:   Must create a device, acts as an interface between the application and the graphics hardware. We use Device
///             whenever we want to allocate resources like a texture, buffer, shader, etc...
/// - CONTEXT:  We use a context to issue draw commands
class Graphics
{
public:
    /// @brief  Exception class for the window
    class Exception : public RomanceException
    {
        using RomanceException::RomanceException;
    };

    /// @brief  Exception class that handles HResults
    class HrException : public Exception
    {
    public:
        HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
        char const* what() const override;
        const char* GetType() const noexcept override;
        static std::string TranslateErrorCode(HRESULT hr);
        HRESULT GetErrorCode() const noexcept;
        std::string GetErrorString() const noexcept;
        std::string GetErrorInfo() const noexcept;
    private:
        HRESULT m_hResult;
        std::string info;
    };

    /// @brief  Useful for context calls that dont return an HRESULT but can still throw a message
    class InfoException : public Exception
    {
    public:
        InfoException(int line, const char* file, std::vector<std::string> infoMsgs = {}) noexcept;
        char const* what() const override;
        const char* GetType() const noexcept override;
        std::string GetErrorInfo() const noexcept;
    private:
        std::string info;
    };

    /// @brief  Specific device removed exception 
    class DeviceRemovedException : public HrException
    {
        using HrException::HrException;
    public:
        const char* GetType() const noexcept override;
    private:
        std::string reason;
    };
    
public:
    Graphics(HWND hWnd);
    ~Graphics() = default;
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) const = delete;

    /// @brief  Presents the back buffer to the front buffer (so to the screen)
    void SwapBuffer();

    /// @brief  Clears our RTV with the specified color
    void ClearBuffer(float r, float g, float b) noexcept;

    void DrawTestTriangle(float dT);

    void SetupCBuffers(float dT);

    void CompileShader(LPCWSTR path, LPCSTR entryPoint, LPCSTR profile, ID3DBlob** ppBlob);
    
private:
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;

    // Buffers
    Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuffer;

#if NDEBUG
    DxgiInfoManager m_InfoManager;
#endif 
};
