cbuffer cbuff
{
    matrix transform;
};

struct vs_out
{
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

vs_out main(float3 pos : POSITION, float4 color : COLOR)
{
    vs_out vs_out;
    vs_out.pos = mul(float4(pos, 1.0f), transform);
    vs_out.color = color;
    return vs_out;
}