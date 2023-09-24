#include "Graphics.h"

// Specify linking to d3d11 library (here instead of project settings)
#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
    // Set up configuration struct for our swap chain
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;                                                /* Buffer Width, 0 to default to window */
    sd.BufferDesc.Height = 0;                                               /* Buffer Height, 0 to default to window */
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                      /* Buffer Format, pixel layout (RGBA, 16) */
    sd.BufferDesc.RefreshRate.Numerator = 0;                                /* Default refresh rate values for buffer front/back swap */
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                  /* Scaling method, none since we default to the window */
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;  /* Scaline order, useful if display is interlaced */
    sd.SampleDesc.Count = 1;                                                /* AntiAliasing, 1 sample with 0 quality meaning no AA */
    sd.SampleDesc.Quality = 0;                                              /* AA quality 0, disabled */
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                       /* Buffer usage, to render target output */
    sd.BufferCount = 1;                                                     /* = 1 means double buffering, front buffer and back buffer */
    sd.OutputWindow = hWnd;                                                 /* Window handle to draw on */
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;                               /* How to handle front buffer after presenting a surface, we specify to discard it after */
    sd.Flags = 0;
    
    // Create the device now and swap chain
    D3D11CreateDeviceAndSwapChain(
        nullptr,                 /* Graphics adapter, setting to null lets it choose it for us */
        D3D_DRIVER_TYPE_HARDWARE,        /* Driver type, whether software acceleration or hardware. Hardware for us */
        nullptr,                 /* Must be null if DriverType is not software (its hardware for us). Handle to binary to our graphics driver to use */
        0,                          /* Various creation flags */
        nullptr,            /* Feature level to support (D3D9, 10, 11, etc..), null results in a default feature level (see docs) */
        0,                   /* Number of feature levels being used, guessing if the above is null, it'll auto this */
        D3D11_SDK_VERSION,    /* SDK Version, self explanatory */
        &sd,                            /* Configuration struct for our swap chain */
        &pSwapChain,                    /* Pointer to our swap chain, to be filled out upon creation */
        &pDevice,                       /* Pointer to our ID3D11 device, to be filled out upon creation */
        nullptr,            /* Out Pointer, would fill out with the feature level actually secured */
        &pContext                       /* Pointer to our device context, to be filled out upon creation */
        );


    // Gain access to texture subresource in swap chain (back buffer)
    ID3D11Resource* pBackBuffer = nullptr;
    // Similar to QueryInterface from COM in terms of inputs, except we're querying a resource on the interface
    pSwapChain->GetBuffer(
        0,                                          /* Index of buffer we wanna get, 0 gives us the back buffer*/
        UUID(ID3D11Resource),                       /* UUID thing of COM interface, a D3D11 resource in our case */
        reinterpret_cast<void**>(&pBackBuffer)      /* PP to our resource to be filled out */
        );

    // Create a reference to the render target view to access and modify later
    pDevice->CreateRenderTargetView(
        pBackBuffer,    /* Buffer in which to receive the render target from */
        nullptr,  /* Config struct to specify how we wanna receive the RTV, default it */
        &pTarget        /* ID3D11 Target to be filled out */
        );

    // Don't need the back buffer we created, was just temporary to retrieve the RTV
    pBackBuffer->Release();
}

Graphics::~Graphics()
{
    if (pContext != nullptr)
    {
        pContext->Release();
    }
    if (pSwapChain != nullptr)
    {
        pSwapChain->Release();
    }
    if (pDevice != nullptr)
    {
        pDevice->Release();
    }
}

void Graphics::SwapBuffer()
{
    pSwapChain->Present(1, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
    const float Color[] = {r, g, b, 1.f};
    pContext->ClearRenderTargetView(pTarget, Color);
}
