struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: TEXCOORD0;
	float2 uv		    : TEXCOORD1;
	float3 tangent		: TEXCOORD2;
};

cbuffer geoBuffer : register(b0)
{
	float age;
};

[maxvertexcount(10)]
void main(
	point VertexToPixel input[1] : SV_POSITION, 
	inout TriangleStream< VertexToPixel > output
)
{
	float r = 0.1;
	float4 pos = input[0].position;
		float4 v[4] = {
		pos + float4(r, -r, 0, 0),
		pos + float4(-r*5 + sin(age*pos.y*pos.y*3)*r, sin(age*pos.y*pos.y *3)*r/2, 0, 0),
		pos + float4(r, r, 0, 0),
		pos + float4(-r*5 + sin(age*pos.y*pos.y*3)*r, sin(age*pos.y*pos.y*3)*r/2, 0, 0),
	};

	float2 uv[4] = {
		float2(0, 0),
		float2(1, 0),
		float2(0, 1),
		float2(1, 1),
	};

	for (uint i = 0; i < 4; i++)
	{
		VertexToPixel element;
		element = input[0];
		element.position = v[i];
		element.uv = uv[i];
		output.Append(element);
	}
}