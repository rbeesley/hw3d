cbuffer CBuf
{
    matrix transform;
};

struct vs_out
{
    float4 color : COLOR;
    float4 pos : SV_Position;
};

vs_out main(float3 pos : POSITION, float4 color : COLOR)
{
    vs_out vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.color = color;
    return vso;
}