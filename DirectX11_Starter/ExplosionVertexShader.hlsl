struct GSInput
{
	float4 pos : SV_POSITION;
	float4 direction : TEXCOORD;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 direction : TEXCOORD0;
	float age : TEXCOORD1;
};

cbuffer asteroidBuffer: register(b0)
{
	float age;
	float3 position;
	float3 acceleration;
};

GSOutput main(GSInput pos)
{
	GSOutput output;
	output.pos = pos.pos;
	//output.pos.z = 100;
	output.direction = pos.direction;
	output.age = age;
	//output.pos *= 10;
	//output.pos = float4(position, 1);
	//matrix worldViewProj = mul(mul(world, view), projection);
//	output.pos = mul(float4(position, 1), worldViewProj);
	output.pos = pos.pos + pos.direction*age*age*0.25;
	return output;
}