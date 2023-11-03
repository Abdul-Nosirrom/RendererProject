
cbuffer Transform 
{
    matrix transform;
}

struct VSOut
{
    float4 Pos : SV_Position;
};

VSOut VSMain(float3 position : Position)
{
    VSOut vs;
    vs.Pos = mul(float4(position, 1.f), transform);
    return vs;
}