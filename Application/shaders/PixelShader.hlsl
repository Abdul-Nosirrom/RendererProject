
struct Interpolants
{
    float4 Pos : SV_Position;
    float Offset : TEXCOORD0;
    uint ID : SV_PrimitiveID;
};

float4 PSMain(Interpolants PSIn) : SV_Target
{
   // return float4((PSIn.ID/2 +1)/8.f * float3(1.f, 0.2f, 0.2f), 1.f);
   return float4(0.f, 0.f, 0.7f, 1.f) * (PSIn.Offset); 
}