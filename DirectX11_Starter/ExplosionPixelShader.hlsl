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
	float r = 0.5;
	float2 uv = input.uv -float2(r, r);
	float dist = length(uv);
	if (dist < r) {
		float2 cs = twirl(dist, input.age/4);
		uv = float2(dot(uv, float2(cs.x, -cs.y)), dot(uv, cs));
	}
	uv = mul(uv, rotate(input.age));
	uv += float2(r, r);
	float4 color = myTexture.Sample(mySampler, uv);
	/*float4 samples[4];
	samples[0] = myTexture.Sample(mySampler, input.uv + float2(1, 0));
	samples[1] = myTexture.Sample(mySampler, input.uv + float2(-1,0));
	samples[2] = myTexture.Sample(mySampler, input.uv + float2(0, 1));
	samples[3] = myTexture.Sample(mySampler, input.uv + float2(0, -1));*/
	//return saturate(float4(color.r + color.g, color.g, color.r, min(color.w, 0.25)));
	return saturate(float4(color.r*2 + color.a*color.r, color.g/2 + color.a *color.g, color.b/4 + color.b * color.a, color.a));
}