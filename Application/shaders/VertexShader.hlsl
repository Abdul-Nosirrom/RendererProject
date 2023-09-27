float4 VSMain(float2 position : Position) : SV_Position
{
#if DO_OFFSET
    position += 0.5f;
#endif 
    return float4(position.x, position.y, 0.f, 1.f);
}