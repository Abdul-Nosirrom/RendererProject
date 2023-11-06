#include "Graphics.h"

#include <sstream>
#include <dxgitype.h>
#include <winerror.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include "DxgiMessageMap.h"
#include "Window.h"
#include "Errors/GraphicsErrors.h"

// namespace for our com ptrs
namespace wrl = Microsoft::WRL;

// Specify linking to d3d11 library (here instead of project settings)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")

/*--------------------------------------------------------------------------------------------------------------
* Exception Class
*--------------------------------------------------------------------------------------------------------------*/

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
        << "[Description] " << Window::Exception::TranslateErrorCode(m_hResult) << std::endl
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
    return Window::Exception::TranslateErrorCode(hr);
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return m_hResult;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    static DxgiMessageMap messageMap;
    bool bFoundMsg;
    std::string errorString = messageMap(m_hResult, bFoundMsg);

    if (bFoundMsg) return errorString;

    return Window::Exception::TranslateErrorCode(HRESULT_CODE(m_hResult));
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return info;
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
    : Exception(line, file)
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

char const* Graphics::InfoException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl
        << GetOriginString();
    m_whatBuffer = oss.str();
    return m_whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
    return "Romance Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
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
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                  /* Buffer Format, pixel layout (RGBA, 16) */
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
    
#ifndef NDEBUG 
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
    //wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
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

    // Create a depth stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    wrl::ComPtr<ID3D11DepthStencilState> pDSState;
    GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

    // Set depth stencil state (Output merger, discards pixels before putting it into the frame buffer)
    pContext->OMSetDepthStencilState(pDSState.Get(), 1u);
    
    // Create a depth stencil texture
    D3D11_TEXTURE2D_DESC dtDesc = {};
    dtDesc.Format = DXGI_FORMAT_D32_FLOAT; // D32 is specific for depth values
    dtDesc.Width = 800u; // Matching viewport
    dtDesc.Height = 600u;
    dtDesc.MipLevels = 1u; // no mipmapping
    dtDesc.ArraySize = 1u; // no mipmapping
    dtDesc.SampleDesc.Count = 1u; // no AA
    dtDesc.SampleDesc.Quality = 0u; // no AA
    dtDesc.Usage = D3D11_USAGE_DEFAULT;
    dtDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    
    // Depth generated every frame so no need to store its data
    GFX_THROW_INFO(pDevice->CreateTexture2D(&dtDesc, nullptr, &pDSTexture));
    
    // Create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0u;

    GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, &pDSV));

    // Bind depth stencil view to the pipeline, output merger for same reason mentioned above
    pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

    // Configure viewport
    m_ViewPort = {};
    m_ViewPort.Width = 800;
    m_ViewPort.Height = 600;
    m_ViewPort.MinDepth = 0;
    m_ViewPort.MaxDepth = 1;
    m_ViewPort.TopLeftX = 0;
    m_ViewPort.TopLeftY = 0;
    pContext->RSSetViewports(1u, &m_ViewPort);

    // Create window associaton
    //wrl::ComPtr<IDXGIFactory> pFactory;
    //CreateDXGIFactory(__uuidof(IDXGIFactory), (&pFactory) );
    //pFactory->MakeWindowAssociation(hWnd, 0u);
}

void Graphics::SwapBuffer()
{
#ifndef NDEBUG
    m_InfoManager.Set(); // To only get latest debug messages
#endif 
    GFX_DEVICE_REMOVED_EXCEPT(pSwapChain->Present(1, 0u))
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
    const float Color[] = {r, g, b, 1.f};
    pContext->ClearRenderTargetView(pTarget.Get(), Color);
    // Clear depth buffer
    pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
    // Bind render target (Output merger)
    pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());
    pContext->DrawIndexed(count, 0u, 0u);
    //GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjectionMat(Math::FXMMATRIX projectionMat) noexcept
{
    m_ProjectionMat = projectionMat;
}

Math::FXMMATRIX Graphics::GetProjectionMat() const noexcept
{
    return m_ProjectionMat;
}

void Graphics::OnViewPortUpdate(float width, float height) noexcept(!IS_DEBUG)
{
    SetProjectionMat( Math::XMMatrixPerspectiveLH( 1.0f,height / width,0.5f,40.0f ) );

    m_ViewPort.Width = width;
    m_ViewPort.Height = height;

    // Clear existing references to back buffer
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    pContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    pTarget.Reset();
    pDSV.Reset();
    pContext->Flush();
    
    // Resize existing swapchain
    pSwapChain->ResizeBuffers(2u, UINT(width), UINT(height), DXGI_FORMAT_R8G8B8A8_UNORM, 0u);
    
    // Get the new backbuffer texture to use as a render target
    GFX_THROW_INFO(pSwapChain->GetBuffer(0, UUID(ID3D11Resource), (&pBackBuffer)));

    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

    // Create Depth/Stencil buffer and create the Depth Stencil View
    {
        D3D11_TEXTURE2D_DESC dtDesc = {};
        dtDesc.Format = DXGI_FORMAT_D32_FLOAT; // D32 is specific for depth values
        dtDesc.Width = UINT(width); // Matching viewport
        dtDesc.Height = UINT(height);
        dtDesc.MipLevels = 1u; // no mipmapping
        dtDesc.ArraySize = 1u; // no mipmapping
        dtDesc.SampleDesc.Count = 1u; // no AA
        dtDesc.SampleDesc.Quality = 0u; // no AA
        dtDesc.Usage = D3D11_USAGE_DEFAULT;
        dtDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        pDevice->CreateTexture2D(&dtDesc, nullptr, &pDSTexture);
        
        // Create depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0u;

        GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, &pDSV));
    }

    // Reset viewport to proper size
    pContext->RSSetViewports(1, &m_ViewPort);

    // Set render target view/dsv for rednering
    pContext->OMSetRenderTargets(1, pTarget.GetAddressOf(), pDSV.Get());
    
    return;
    /*
    pSwapChain->ResizeBuffers(2u, m_ViewPort.Width, m_ViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0u);

    DXGI_MODE_DESC spDesc = {};
    spDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    spDesc.Width = UINT(m_ViewPort.Width);
    spDesc.Height = UINT(m_ViewPort.Height);
    spDesc.RefreshRate.Numerator = 0;
    spDesc.RefreshRate.Denominator = 0;
    spDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    pSwapChain->ResizeTarget(&spDesc);
    
    pContext->RSSetViewports(1u, &m_ViewPort);*/
}
