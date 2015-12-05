struct GSInput
{
	float4 pos : SV_POSITION;
	float4 direction : TEXCOORD;
};

GSInput main( GSInput pos)
{
	return pos;
}