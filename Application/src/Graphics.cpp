#include "Graphics.h"

#include <sstream>
#include <dxgitype.h>
#include <winerror.h>

// namespace for our com ptrs
namespace wrl = Microsoft::WRL;

// Specify linking to d3d11 library (here instead of project settings)
#pragma comment(lib, "d3d11.lib")

/*--------------------------------------------------------------------------------------------------------------
* Exception Class
*--------------------------------------------------------------------------------------------------------------*/

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_NOINFO(hrcall) if (HRESULT hr1 = hrcall; FAILED(hr1)) throw Graphics::HrException(__LINE__, __FILE__, hr1)

#if NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, hr, m_InfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) m_InfoManager.Set(); if (HRESULT hr1 = hrcall; FAILED(hr1)) throw GFX_EXCEPT(hr1)
#define GFX_DEVICE_REMOVED_EXCEPT(hrcall) if (HRESULT hr2 = hrcall; FAILED(hr2)) { if (hr2 == DXGI_ERROR_DEVICE_REMOVED) throw Graphics::DeviceRemovedException(__LINE__, __FILE__, pDevice->GetDeviceRemovedReason(), m_InfoManager.GetMessages()); else GFX_EXCEPT(hr2); }
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if (HRESULT hr1 = hrcall; FAILED(hr1)) throw GFX_EXCEPT(hr1)
#define GFX_DEVICE_REMOVED_EXCEPT(hrcall) if (HRESULT hr2 = hrcall; FAILED(hr2)) { if (hr2 == DXGI_ERROR_DEVICE_REMOVED) throw Graphics::DeviceRemovedException(__LINE__, __FILE__, pDevice->GetDeviceRemovedReason()); else GFX_EXCEPT(hr2); }
#endif

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
    : Exception(line, file), m_hResult(hr)
{
    // Join all info messages with newline into a single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
        info.push_back('\n');
    }

    // Remove final newline if it exists
    if (!info.empty())
    {
        info.pop_back();
    }
}

char const* Graphics::HrException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() <<  std::dec << " (" << (unsigned long)GetErrorCode() << ") " << std::endl
        << "[Error String] " << GetErrorString()
        << "[Description] " << TranslateErrorCode(m_hResult) << std::endl
        << "[Error Info] " << std::endl << GetErrorInfo() << std::endl
        << GetOriginString();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
    return "Romance Graphics Exception";
}

std::string Graphics::HrException::TranslateErrorCode(HRESULT hr)
{
    char* pMsgBuf = nullptr;
    // Format Message takes hResult and returns a readable error message, filling out our char*. Returns length of error string
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |                 /*  Allocates a buffer for our message, lets pMsgBuf point to it */
        FORMAT_MESSAGE_FROM_SYSTEM |                            /* Searches message table for a requested message. Allows us to use GetLastError() */
        FORMAT_MESSAGE_IGNORE_INSERTS,                          /* Ignores insert formatting (e.g %1), so output message can be formatted later */
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0, nullptr);                             /* nSize zero because we're using Allocate_Buffer, otherwise this would set the size of the output buffer. If Alloc flag is set, this is the min size */

    if (nMsgLen == 0)
    {
        return "Unidentified Error Code";
    }

    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return m_hResult;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    char* pMsgBuf = nullptr;
    // Format Message takes hResult and returns a readable error message, filling out our char*. Returns length of error string
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |                 /*  Allocates a buffer for our message, lets pMsgBuf point to it */
        FORMAT_MESSAGE_FROM_SYSTEM |                            /* Searches message table for a requested message. Allows us to use GetLastError() */
        FORMAT_MESSAGE_IGNORE_INSERTS,                          /* Ignores insert formatting (e.g %1), so output message can be formatted later */
        nullptr,
        HRESULT_CODE(m_hResult),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf),
        0, nullptr);                             /* nSize zero because we're using Allocate_Buffer, otherwise this would set the size of the output buffer. If Alloc flag is set, this is the min size */

    if (nMsgLen == 0)
    {
        return "Unidentified Error Code";
    }

    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "Romance Exception [DEVICE REMOVED] (DXGI_ERROR_DEVICE_REMOVED)";
}

/*--------------------------------------------------------------------------------------------------------------
* Graphics Object
*--------------------------------------------------------------------------------------------------------------*/

Graphics::Graphics(HWND hWnd)
{
    // Set up configuration struct for our swap chain
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;                                                /* Buffer Width, 0 to default to window */
    sd.BufferDesc.Height = 0;                                               /* Buffer Height, 0 to default to window */
    sd.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;                  /* Buffer Format, pixel layout (RGBA, 16) */
    sd.BufferDesc.RefreshRate.Numerator = 0;                                /* Default refresh rate values for buffer front/back swap */
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                  /* Scaling method, none since we default to the window */
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;  /* Scaline order, useful if display is interlaced */
    sd.SampleDesc.Count = 1;                                                /* AntiAliasing, 1 sample with 0 quality meaning no AA */
    sd.SampleDesc.Quality = 0;                                              /* AA quality 0, disabled */
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                       /* Buffer usage, to render target output */
    sd.BufferCount = 2;                                                     /* = 1 means double buffering, front buffer and back buffer (set to 2 because using Flip for swap effect and it said i had to) */
    sd.OutputWindow = hWnd;                                                 /* Window handle to draw on */
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;                          /* How to handle front buffer after presenting a surface, we specify to discard it after */
    sd.Flags = 0;

    UINT swapCreateFlags = 0u;
#if NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 
    
    // Create the device now and swap chain
    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        nullptr,                    /* Graphics adapter, setting to null lets it choose it for us */
        D3D_DRIVER_TYPE_HARDWARE,   /* Driver type, whether software acceleration or hardware. Hardware for us */
        nullptr,                    /* Must be null if DriverType is not software (its hardware for us). Handle to binary to our graphics driver to use */
        swapCreateFlags,            /* Various creation flags, here we specify to create the device on the debug layer so we can retrieve detailed debug outputs */
        nullptr,                    /* Feature level to support (D3D9, 10, 11, etc..), null results in a default feature level (see docs) */
        0,                          /* Number of feature levels being used, guessing if the above is null, it'll auto this */
        D3D11_SDK_VERSION,          /* SDK Version, self explanatory */
        &sd,                        /* Configuration struct for our swap chain */
        &pSwapChain,                /* Pointer to our swap chain, to be filled out upon creation */
        &pDevice,                   /* Pointer to our ID3D11 device, to be filled out upon creation */
        nullptr,                    /* Out Pointer, would fill out with the feature level actually secured */
        &pContext                   /* Pointer to our device context, to be filled out upon creation */
        ));


    // Gain access to texture subresource in swap chain (back buffer)
    wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
    // Similar to QueryInterface from COM in terms of inputs, except we're querying a resource on the interface
    GFX_THROW_INFO(pSwapChain->GetBuffer(
        0,                             /* Index of buffer we wanna get, 0 gives us the back buffer*/
        UUID(ID3D11Resource),          /* UUID thing of COM interface, a D3D11 resource in our case */
        (&pBackBuffer)                 /* PP to our resource to be filled out */
        ));

    // Create a reference to the render target view to access and modify later
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(
        pBackBuffer.Get(),      /* Buffer in which to receive the render target from */
        nullptr,                /* Config struct to specify how we wanna receive the RTV, default it */
        &pTarget                /* ID3D11 Target to be filled out */
        ));
}

void Graphics::SwapBuffer()
{
#if NDEBUG
    m_InfoManager.Set(); // To only get latest debug messages
#endif 
    GFX_DEVICE_REMOVED_EXCEPT(pSwapChain->Present(1, 0u))
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
    const float Color[] = {r, g, b, 1.f};
    pContext->ClearRenderTargetView(pTarget.Get(), Color);
}
