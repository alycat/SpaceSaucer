
cbuffer geoBuffer : register(b0)
{
	float age;
};

struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv		    : TEXCOORD1;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD0;
	float2 uv			: TEXCOORD1;
	float age			: TEXCOORD2;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;
	output.position = float4(input.position, 1.0f);
	output.normal = input.normal;
	output.uv = input.uv;
	output.age = age;
	return output;
}