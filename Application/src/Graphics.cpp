#include "Graphics.h"

#include <sstream>
#include <dxgitype.h>
#include <winerror.h>
#include <d3dcompiler.h>
#include "DxgiMessageMap.h"
#include "Window.h"

// namespace for our com ptrs
namespace wrl = Microsoft::WRL;

// Specify linking to d3d11 library (here instead of project settings)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

/*--------------------------------------------------------------------------------------------------------------
* Exception Class
*--------------------------------------------------------------------------------------------------------------*/

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, hr)
#define GFX_THROW_NOINFO(hrcall) if (HRESULT hr1 = hrcall; FAILED(hr1)) throw Graphics::HrException(__LINE__, __FILE__, hr1)

#if NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, hr, m_InfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) m_InfoManager.Set(); if (HRESULT hr1 = hrcall; FAILED(hr1)) throw GFX_EXCEPT(hr1)
#define GFX_THROW_INFO_ONLY(call) m_InfoManager.Set(); (call); {auto v = m_InfoManager.GetMessages(); if (!v.empty()) { throw Graphics::InfoException(__LINE__, __FILE__, v); } }
#define GFX_DEVICE_REMOVED_EXCEPT(hrcall) if (HRESULT hr2 = hrcall; FAILED(hr2)) { if (hr2 == DXGI_ERROR_DEVICE_REMOVED) throw Graphics::DeviceRemovedException(__LINE__, __FILE__, pDevice->GetDeviceRemovedReason(), m_InfoManager.GetMessages()); else GFX_EXCEPT(hr2); }
#else
#define GFX_EXCEPT(hr) GFX_EXCEPT_NOINFO(hr)
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_THROW_INFO_ONLY(call) (call)
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

void Graphics::DrawTestTriangle(float dT)
{
    SetupCBuffers(dT);
    
    struct Vertex
    {
        float x, y;
        unsigned char r, g, b, a;
    };

    const Vertex vertices[] = {
        {-0.25f, -0.25f, 255, 0, 0}, // 0 Bottom Left
         {0.25f, -0.25f, 255, 255, 0}, // 1 Bottom Right
        {-0.25f,  0.25f, 0, 0, 255}, // 2 Top Left
         {0.25f,  0.25f, 0, 255, 0}  // 3 Top Right
    };

    // NOTE: Winding number is important, unless culling is disabled in RasterizerState, will auto cull back faces
    const int indices[] =
        {
            2, 1, 0,
            1, 2, 3
        };

    // Setup buffer description struct
    D3D11_BUFFER_DESC bd;
    bd.ByteWidth = sizeof(vertices);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.StructureByteStride = sizeof(Vertex);
    

    //
    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = vertices;
    
    //
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    
    // Need to allocate a vertex buffer
    GFX_THROW_INFO(pDevice->CreateBuffer(
        &bd,
        &sd,
        &pVertexBuffer
        ));

    // Set the vertex buffer
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    pContext->IASetVertexBuffers
    (
        0u,
        1u,
        pVertexBuffer.GetAddressOf(),
        &stride,
        &offset
        );

    // Set the index buffer
    D3D11_BUFFER_DESC ibd;
    ibd.ByteWidth = sizeof(indices);
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.StructureByteStride = sizeof(int);

    D3D11_SUBRESOURCE_DATA isd;
    isd.pSysMem = indices;

    wrl::ComPtr<ID3D11Buffer> pIndexBuffer;

    GFX_THROW_INFO(pDevice->CreateBuffer(
        &ibd,
        &isd,
        &pIndexBuffer));

    pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

    //pContext->IASetIndexBuffer()

    // Create vertex shader COM object
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    wrl::ComPtr<ID3DBlob> pVSBlob;
    
    CompileShader(L"shaders/VertexShader.hlsl", "VSMain", "vs_5_0", &pVSBlob);

    // Create the shader object
    GFX_THROW_INFO(pDevice->CreateVertexShader(
        pVSBlob->GetBufferPointer(),
        (SIZE_T)pVSBlob->GetBufferSize(),
        nullptr,
        &pVertexShader));

    // Bind shader to pipeline
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

    // Specify the layout of our buffer (Need to do after compiling shader since it takes the blob)
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;

    const D3D11_INPUT_ELEMENT_DESC ied[] =
        {
            {
                "Position",
                0u,
                DXGI_FORMAT_R32G32_FLOAT,               // 2 Floats specifying a single element
                0u,
                D3D11_APPEND_ALIGNED_ELEMENT ,
                D3D11_INPUT_PER_VERTEX_DATA,
                0u
            },
            {
                "Color",
                0u,
                DXGI_FORMAT_R8G8B8A8_UNORM,             // UNORM means when it does its type conversion to the shader, it'll normalize the value. E.g unsigned char [0,255] gets converted (if the shader specifies a float for the input) to float [0,1]
                0u, D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0u
            }
        };
    
    GFX_THROW_INFO(pDevice->CreateInputLayout(
        ied, std::size(ied),
        pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
        &pInputLayout));
    
    pContext->IASetInputLayout(pInputLayout.Get());
    
    // Create pixel shader COM object
    wrl::ComPtr<ID3D11PixelShader> pPixelShader;

    CompileShader(L"shaders/PixelShader.hlsl", "PSMain", "ps_5_0", &pVSBlob);
    
    GFX_THROW_INFO(pDevice->CreatePixelShader(
        pVSBlob->GetBufferPointer(),
        (SIZE_T)pVSBlob->GetBufferSize(),
        nullptr,
        &pPixelShader));

    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

    // Bind render target (Output merger)
    pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

    // Set primitive topology to triangle list
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    // Configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports(1u, &vp);
    
    //GFX_THROW_INFO_ONLY(pContext->Draw(std::size(vertices), 0u));
    pContext->DrawIndexed(std::size(indices), 0u, 0u);
}

void Graphics::SetupCBuffers(float dT)
{
    struct PS_CBUFFER
    {
        float r, g, b, a;
    } cbufData;

    //PS_CBUFFER cbufData;
    cbufData.r = sin(dT) / 2.f + 0.5f;;
    cbufData.g = cos(dT) / 2.f + 0.5f;;
    cbufData.b = 0.f;

    static bool bResourceCreated = false;

    if (!bResourceCreated)
    {
        //wrl::ComPtr<ID3D11Buffer> pCBuffer;

        // For buffers that are frequently updated, usage = dynamic and CPU access = write
        D3D11_BUFFER_DESC bd;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0u;
        bd.ByteWidth = sizeof(cbufData);
        bd.StructureByteStride = 0u;//sizeof(PS_CBUFFER);

        D3D11_SUBRESOURCE_DATA sd;
        sd.pSysMem = &cbufData;
    
        // First need to create the buffer
        GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pCBuffer));

        pContext->PSSetConstantBuffers(0u, 1u, pCBuffer.GetAddressOf());

        bResourceCreated = true;
    }
    else
    {
        // Resource already sent to the GPU, we just need to modify it
        D3D11_MAPPED_SUBRESOURCE msd;
        ZeroMemory(&msd, sizeof(D3D11_MAPPED_SUBRESOURCE));

        GFX_THROW_INFO(pContext->Map(pCBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0, &msd));

        // We got the cbuffer data from the gpu, update its data
        //msd.pData = &cbufData;
        memcpy(msd.pData, &cbufData, sizeof(cbufData));

        pContext->Unmap(pCBuffer.Get(), 0u);
    }
}

void Graphics::CompileShader(LPCWSTR path, LPCSTR entryPoint, LPCSTR profile, ID3DBlob** ppBlob)
{
    // Compile shader
    wrl::ComPtr<ID3DBlob> pErrorBlob;

    const D3D_SHADER_MACRO defines[] =
    {
        "DO_OFFSET", "1",
        NULL, NULL
    };
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;// | D3DCOMPILE_DEBUG;
    //LPCSTR profile = (pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "vs_5_0" : "vs_4_0";

    // NOTE: Shader file encoding has to be ANSI not UNICODE, go to file->file encoding->ISO-8859-1 for hlsl files
    HRESULT compHR = D3DCompileFromFile(
        path,
        defines,
        nullptr,
        entryPoint, profile,
        flags, 0, ppBlob, &pErrorBlob);

    if (compHR < 0)
    {
        auto err = (char*)pErrorBlob->GetBufferPointer();
        throw HrException(__LINE__, __FILE__, compHR, {err});
    }
}
