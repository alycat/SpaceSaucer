struct GSOutput
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD0;
	float2 uv			: TEXCOORD1;
	float age			: TEXCOORD2;
};

cbuffer perModel		: register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

void createParticle(GSOutput input, inout TriangleStream<GSOutput> output){

	matrix worldViewProj = mul(mul(world, view), projection);
	float4 v[4];
	float4 position = input.position;
	float r = 5;
	v[0] = position + float4(r, -r, 0.0f, 0.0f);
	v[1] = position + float4(-r, -r, 0.0f, 0.0f);
	v[2] = position + float4(r, r, 0.0f, 0.0f);
	v[3] = position + float4(-r, r, 0.0f, 0.0f);

	float2 quadUVs[4] = {
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f)
	};

	GSOutput element = input;
	for (uint i = 0; i < 4; i++)
	{
		element.position = mul(v[i], worldViewProj);
		element.uv = quadUVs[i];
		output.Append(element);
	}
	output.RestartStrip();
}

[maxvertexcount(100)]
void main(
	point GSOutput input[1] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{

	matrix worldViewProj = mul(mul(world, view), projection);

	GSOutput element = input[0];
	createParticle(element, output);
}