Texture2D myTexture: register(t0);
SamplerState mySampler: register(s0);

cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer resolution: register(b1)
{
	float3 resolution;
	float empty;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD0;
	float2 uv			: TEXCOORD1;
	float2 pos			: TEXCOORD2;
	float age			: TEXCOORD3;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	float3 lightDir = float3(0, 0, 1);
	lightDir = normalize(input.position);
	//lightDir = normalize(input.position);
	float3 viewDir = float3(0, 0, 1);
	viewDir = normalize(input.position);

	float4 diffuse = saturate(dot(lightDir, input.normal));
	float3 reflection = reflect(-lightDir, input.normal);
	float4 specular = pow(saturate(dot(reflection, viewDir)), 0.5);

	//color = myTexture.Sample(mySampler,(float2(input.position.x/800, input.position.y/600)));
	//color += float4(0.5, 0.5, 0.5, 0.0);
//	color.w = 1;
	float3 I = float3(0, 0, 1);
	float3 N = normalize(input.normal);
	float inc = 1.05;
	float d = dot(I, N);
	if (d <= 0){
		N *= -1;
		d = dot(N, I);
	}
	float value = 1 - ((inc*inc)*(1 - (d*d)));
	value = value > 0 ? sqrt(value) : 0;
	float3 t = value > 0 ? (I - N * d * inc + N * value) : I;
	float4 refraction = myTexture.Sample(mySampler, float2((input.position.x / resolution.x) + (t.x * 2), (input.position.y / resolution.y) + (t.y * 2)));
	float4 color = (lerp(refraction, float4(0.75,0.25,1,1), 0.75) * 0.33) + (float4(0, 1, 1, 1) * diffuse * 0.5) + (specular * float4(0, 1, 1, 1) * 0.2);
	color = lerp(refraction, color, 0.5);
	color.w = 1;
	return saturate(color);
}