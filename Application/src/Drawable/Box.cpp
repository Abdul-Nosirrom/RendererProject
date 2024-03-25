#include "Box.h"

#include "Bindable/BindableCommon.h"
#include "Utility/IndexedTriangleList.h"
#include "Utility/ShapesCommon.h"

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
    if (!IsStaticInitialized())
    {
        IndexedTriangleList<Vertex> model = Plane::MakeTesselated<Vertex>(128, 128);

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

        AddSharedBindable(std::make_unique<VertexBuffer>(gfx, model.m_Vertices));

        AddSharedBindable(std::move(pVS));
        AddSharedBindable(std::make_unique<PixelShader>(gfx, L"shaders/PixelShader.hlsl"));

        AddSharedIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.m_Indices));

        AddSharedBindable(std::make_unique<InputLayout>(gfx, ied, pVSB));

        AddSharedBindable(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));
    }
    else
    {
        SetIndexBufferFromSharedBindables();
    }

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
    Math::XMMatrixScaling(10.f, 10.f, 1.f) *
    Math::XMMatrixRotationRollPitchYaw( Math::PI/3.f,0.f,0.f ) *
    Math::XMMatrixTranslation(0.f ,0.f, 20.f);
    return
        Math::XMMatrixRotationRollPitchYaw( pitch,yaw,roll ) *
        Math::XMMatrixTranslation( r,0.0f,0.0f ) *
        Math::XMMatrixRotationRollPitchYaw( theta,phi,chi ) *
        Math::XMMatrixTranslation( 0.0f,0.0f,20.0f );
}
