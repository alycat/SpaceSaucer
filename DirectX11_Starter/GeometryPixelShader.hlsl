Texture2D myTexture: register(t0);
SamplerState mySampler: register(s0);

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 pos			: TEXCOORD0;
	float2 uv			: TEXCOORD1;
};

float4 main(in VertexToPixel input) : SV_TARGET
{
	float4 color = myTexture.Sample(mySampler, input.uv);
	color.w = (1.2/input.position.z) - 1;
	return color;
}