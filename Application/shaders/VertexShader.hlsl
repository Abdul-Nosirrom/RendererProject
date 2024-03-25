

cbuffer Transform
{
    matrix transform;
}

struct VSOut
{
    float4 Pos : SV_Position;
    float Offset : TEXCOORD0;
};

VSOut VSMain(float3 position : Position)
{
    VSOut vs;
    float sinIn1 = position.x;
    float sinIn2 = position.y;
    vs.Offset = (0.7f * sin(18.f  * sinIn1) + 0.5f * sin(24.f * sinIn2) + 0.6f * sin(42.f * sinIn1) + 0.2f * sin(64.f * sinIn2)  + 2.f) / 4.f;
    position += float3(0.f, 0.f, -1.f) * vs.Offset;
    
    vs.Pos = mul(float4(position, 1.f), transform);
    return vs;
}