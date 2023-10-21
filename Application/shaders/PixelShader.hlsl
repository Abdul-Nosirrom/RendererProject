cbuffer PS_CBUFFER : register(b0)
{
    float r, g, b, a;
}

struct Interpolants
{
    float4 Pos : SV_Position;
    float3 Color : Color;
};


float4 PSMain(Interpolants PSIn) : SV_Target
{
    
    return float4(PSIn.Color, 1.f);
}