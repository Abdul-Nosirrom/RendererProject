#include "Box.h"

#include "Bindable/BindableCommon.h"

Box::Box(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist,
         std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist,
         std::uniform_real_distribution<float>& rdist)
    :
    r( rdist( rng ) ),
    droll( ddist( rng ) ),
    dpitch( ddist( rng ) ),
    dyaw( ddist( rng ) ),
    dphi( odist( rng ) ),
    dtheta( odist( rng ) ),
    dchi( odist( rng ) ),
    chi( adist( rng ) ),
    theta( adist( rng ) ),
    phi( adist( rng ) )
{
    const float Size = 1.f;
    const std::vector<Vertex> vertices = {
        {-Size, -Size,  -Size}, // 0
        {Size, -Size, -Size}, // 1
        {-Size,  Size, -Size}, // 2
        {Size,  Size, -Size},  // 3
        {-Size, -Size,  Size}, // 4
        {Size, -Size, Size}, // 5
        {-Size,  Size, Size}, // 6
        {Size,  Size, Size}  // 7
    };

    // NOTE: Winding number is important, unless culling is disabled in RasterizerState, will auto cull back faces
    const std::vector<unsigned short> indices =
        {
        // Front Face Quad (Fixed (-) Z)
        0, 2, 1,
        2, 3, 1,
        // Back Face Quad (Fixed (+) Z)
        4, 5, 7,
        4, 7, 6,
        // Right Face Quad (Fixed (+) X)
        1, 3, 5,
        3, 7, 5,
        // Left Face Quad (Fixed (-) X)
        0, 4, 2,
        2, 4, 6,
        // Top Face Quad (Fixed (+) Y)
        2, 6, 3,
        3, 6, 7,
        // Bottom Face Quad (Fixed (-) Y)
        0, 1, 4,
        1, 5, 4
    };

    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    {
        {
            "Position",
            0u,
            DXGI_FORMAT_R32G32B32_FLOAT,               // 3 Floats specifying a single element
            0u,
            D3D11_APPEND_ALIGNED_ELEMENT ,
            D3D11_INPUT_PER_VERTEX_DATA,
            0u
        }
    };

    auto pVS = std::make_unique<VertexShader>(gfx, L"shaders/VertexShader.hlsl");
    auto pVSB = pVS->GetBytecode();

    AddBind(std::make_unique<VertexBuffer>(gfx, vertices));

    AddBind(std::move(pVS));
    AddBind(std::make_unique<PixelShader>(gfx, L"shaders/PixelShader.hlsl"));

    AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

    AddBind(std::make_unique<InputLayout>(gfx, ied, pVSB));

    AddBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    AddBind( std::make_unique<TransformCBuffer>( gfx,*this ) );
}

void Box::Update(float dt) noexcept
{
    roll += droll * dt;
    pitch += dpitch * dt;
    yaw += dyaw * dt;
    theta += dtheta * dt;
    phi += dphi * dt;
    chi += dchi * dt;
}

Math::XMMATRIX Box::GetTransformMat() const noexcept
{
    return
        Math::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
        Math::XMMatrixTranslation( r,0.0f,0.0f ) *
        Math::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
        Math::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}
