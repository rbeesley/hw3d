struct ps_in
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

float4 main(ps_in pin) : SV_TARGET
{
	return pin.color;
}