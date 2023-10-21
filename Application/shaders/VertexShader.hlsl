
cbuffer Transform : register(b0)
{
    matrix transform;
}

struct VSOut
{
    float4 Pos : SV_Position;
    float3 Color : Color;
};

VSOut VSMain(float3 position : Position)
{
#if DO_OFFSET
    position += 0.5f;
#endif
    VSOut vs;
    vs.Pos = mul(float4(position, 1.f), transform);
    vs.Color = float4(1.f, 1.f, 1.f, 1.f);
    return vs;
}