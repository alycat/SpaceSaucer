Texture2D myTexture: register(t0);
Texture2D bumps: register(t1);
Texture2D highlights: register(t2);
SamplerState mySampler: register(s0);


// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		 : SV_POSITION;
	float2 uv			 : TEXCOORD0;
	float age			 : TEXCOORD1;
};

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float specularPower;
	float4 specularColor;
	float4 textureColor;
};

float2x2 rotate(float angle) {
	float x = cos(angle);
	float y = sin(angle);
	return float2x2(x, -y, y, x);
}

float2 twirl(float dist, float angle) {
	float percent = (0.5 - dist)/0.5;
	float theta = percent*percent*angle*8.0;
	float y = sin(theta);
	float x = cos(theta);
	return float2(x, y);
}
// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	return myTexture.Sample(mySampler, input.uv);
}