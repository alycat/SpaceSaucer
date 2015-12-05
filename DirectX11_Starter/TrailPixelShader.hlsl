struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	float u = input.uv.x;
	float v = input.uv.y;
	float opacity = v;
	//input.color.w *= (input.uv.y + input.color.w);
	//input.color.w /= 2;
	return input.color;
}