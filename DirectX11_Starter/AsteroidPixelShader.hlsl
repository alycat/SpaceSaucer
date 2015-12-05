Texture2D myTexture: register(t0);
Texture2D bumps: register(t1);
Texture2D highlights: register(t2);
SamplerState mySampler: register(s0);


// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		 : SV_POSITION;
	//float3 posW          : POSITION;
	float3 normal		 : NORMAL;
	float2 uv			 : TEXCOORD0;
	float3 tangent		 : TEXCOORD1;
	float4 direction	 : TEXCOORD2;
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

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Interpolating normal can unnormalize it, so normalize it.
	float3 lightDir = normalize(cross(-input.direction.xyz, float3(0, -1, 1)));
	lightDir = -lightDirection;
	lightDir = normalize(input.position.xyz);
	float3 normal = normalize(input.normal);
	float3 tangent = normalize(input.tangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	float3 bitangent = normalize(cross(tangent, normal));
		//Normal Mapping
		float4 lava = (saturate(highlights.Sample(mySampler, input.uv) * float4(0.5, 0.2, 0.1, 1)));
		normal = (bumps.Sample(mySampler, input.uv).rgb * 2.0) - float3(1, 1, 1);
//	normal = normalize(normal);
	float3x3 mat = (float3x3(tangent, bitangent, normal));
	normal = mul(mat, normal);
	normal -= lava.xyz*1.5;
	normal = normalize(normal);
	//float3 bumpedNormalW = NormalSampleToWorldSpace(
	//normalMapSample, input.normal, tangent);
	//normal = normalize(input.normal);
	float3 reflection = normalize(reflect(lightDir, normal));
	float4 textureColor = myTexture.Sample(mySampler, input.uv);
	float4 specular = pow(saturate(dot(reflection, lightDir)), specularPower) * specularColor;
	float4 diffuse = lerp(diffuseColor, textureColor, 0) * saturate(dot(normal, -lightDir));
	float intensity = dot(normalize(-lightDir), normal);
	float4 color = saturate(diffuse*0.5 + ambientColor*0.33 + specular*0.25);
	//float4 color = diffuse*0.5;
//	color.a = 1;
	color += lava*0.5;
	
	if (intensity > 0.95)
		color *= specular;
	else if (intensity > 0.5)
		color *= diffuse;
	else if (intensity > 0.02)
		color *= ambientColor;
	else
		color *= ambientColor;
		
	float angle = acos(dot(normalize(input.normal), float3(0,0,1)));
	float diff = abs(angle - 1.57079632679);
	if (diff < 0.25){
		color += float4((0.5 - diff)*(0.5 - diff), 0, 0, 1);
	}
	color.a = 1;
	return saturate(color);
}