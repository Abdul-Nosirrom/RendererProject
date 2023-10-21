
struct Interpolants
{
    float4 Pos : SV_Position;
    uint ID : SV_PrimitiveID;
};


float4 PSMain(Interpolants PSIn) : SV_Target
{
    return float4((PSIn.ID/2 +1)/8.f * float3(1.f, 0.2f, 0.2f), 1.f);
}