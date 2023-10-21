
struct VSOut
{
    float4 Pos : SV_Position;
    float3 Color : Color;
};

VSOut VSMain(float2 position : Position, float3 Color : Color)
{
#if DO_OFFSET
    position += 0.5f;
#endif
    VSOut vs;
    vs.Pos = float4(position.x, position.y, 0.f, 1.f);
    vs.Color = Color;
    return vs;
}