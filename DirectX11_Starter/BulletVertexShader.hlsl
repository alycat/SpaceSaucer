//http://www.gamasutra.com/view/feature/131275/implementing_lighting_models_with_.php?print=1


// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInput
{
	float3 position		: POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: TEXCOORD0;
	float2 uv		    : TEXCOORD1;
	float3 tangent		: TEXCOORD2;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: TEXCOORD0;
	float2 uv		    : TEXCOORD1;
	float3 tangent		: TEXCOORD2;
};

// The entry point for our vertex shader
VertexToPixel main(VertexInput input)
{
	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);
	output.normal = input.normal;
	output.uv = input.uv;
	output.tangent = input.tangent;
	return output;
}