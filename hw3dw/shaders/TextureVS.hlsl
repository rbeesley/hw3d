cbuffer cbuff
{
	matrix wvp;
};

struct vs_out
{
	float2 tex : TEXCOORD;
    float4 pos : SV_POSITION;
};

vs_out main(float3 pos : POSITION, float2 tex : TEXCOORD)
{
	vs_out vs_out;
	vs_out.pos = mul(float4(pos, 1.0f), wvp);
	vs_out.tex = tex;
	return vs_out;
}