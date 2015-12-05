Texture2D myTexture: register(t0);
SamplerState mySampler: register(s0);


// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: TEXCOORD0;
	float2 uv		    : TEXCOORD1;
	float3 tangent		: TEXCOORD2;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{	// Just return the input color
	// - Note that this color (like all values that pass through the rasterizer)
	//   is interpolated for each pixel between the corresponding 
	//   vertices of the triangle

	//return myTexture.Sample(mySampler, input.uv);
	float4 color = float4(0,0,0,0);
	float a = 1;
	float b = 1;
	float y = (input.uv.y*-1) + 0.5;
	float x = 1 - input.uv.x;
	float difference = ((x*x*x)*(a - x)) - ((b*b)*(y*y));
	if (difference >= 0)
	{
		color = float4(15/(1+difference), difference*20, difference*15, 0.33);
	}
	return saturate(color);
}