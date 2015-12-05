/*cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};*/

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


	// Calculate output position
//	matrix worldViewProj = mul(mul(world, view), projection);
	float x = input.position.x;
	float y = input.position.y;
	float z = input.position.z;
	output.position = float4(x, y, z, 1);// float4(input.position, 1.0f);// mul(float4(input.position, 1.0f), worldViewProj);
	output.uv = input.uv;
	output.normal = float3(0, 0, 1);//input.normal;
	output.age = age;

	return output;
}