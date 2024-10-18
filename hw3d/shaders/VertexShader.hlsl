struct vs_out
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer cbuff
{
    matrix transform;
};

vs_out main( float4 pos : POSITION, float4 color : COLOR )
{
    vs_out vout;
    vout.pos = mul(pos, transform);
    vout.color = color;
	return vout;
}