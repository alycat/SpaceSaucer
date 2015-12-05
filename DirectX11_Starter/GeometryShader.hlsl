struct VSOutput
{
	float4 position		: SV_POSITION;
	float4 speed		: COLOR;
};

struct GSOutput
{
	float4 position		: SV_POSITION;
	float2 pos			: TEXCOORD0;
	float2 uv			: TEXCOORD1;
};

cbuffer perModel		: register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

[maxvertexcount(4)]
void main(
	point VSOutput dataIn[1],
	inout TriangleStream< GSOutput > output
	)
{
	matrix worldViewProj = mul(mul(world, view), projection);
	float r = 0.025;
	float4 v[4];
	float4 temp = float4(dataIn[0].position);
	v[0] = temp + float4(r, -r, 0.0f, 0.0f);
	v[1] = temp + float4(-r, -r, 0.0f, 0.0f);
	v[2] = temp + float4(r, r, 0.0f, 0.0f);
	v[3] = temp + float4(-r, r, 0.0f, 0.0f);


	float2 quadUVs[4] = {
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f)
	};

	GSOutput element;
	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		element.position = mul(v[i], worldViewProj);
		element.uv = quadUVs[i];
		element.pos = dataIn[0].position.xy;
		output.Append(element);
	}
	output.RestartStrip();
}