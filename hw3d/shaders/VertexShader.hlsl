struct vs_out
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

vs_out main( float4 pos : POSITION, float4 color : COLOR )
{
    vs_out vout;
    vout.pos = pos;
    vout.color = color;
	return vout;
}