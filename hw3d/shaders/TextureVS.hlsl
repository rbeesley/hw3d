cbuffer CBuf
{
	matrix transform;
};

struct vs_out
{
	float2 tex : TexCoord;
	float4 pos : SV_Position;
};

vs_out main( float3 pos : Position,float2 tex : TexCoord )
{
	vs_out vso;
	vso.pos = mul( float4(pos,1.0f),transform );
	vso.tex = tex;
	return vso;
}