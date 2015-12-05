
cbuffer geoBuffer : register(b0)
{
	float age;
	float3 position;
	float4 acceleration;
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
	float4 acceleration	: TEXCOORD3;
};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;
//	matrix worldViewProj = mul(mul(world, view), projection);
	//output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.position = float4(position/10, 1);
	output.normal = input.normal;
	output.uv = input.uv;
	output.age = age;
	output.acceleration = acceleration;
	return output;
}