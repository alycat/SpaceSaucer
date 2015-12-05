struct GSOutput
{
	float4 position : SV_POSITION;
	float4 direction : TEXCOORD;
};

cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

half noise(int x, int y){
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return saturate((1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 2147483647) / 1073741824.0));
}

[maxvertexcount(128)]
void main(
	point GSOutput input[1],
	inout PointStream< GSOutput > output
	)
{

	matrix worldViewProj = mul(mul(world, view), projection);

	for (uint x = 0; x < 12; ++x){
		for (uint y = 0; y < 12; ++y){
			float u = noise(x, y);
			float v = noise(y, x);
			float phi = 2 * 3.141429 * u;
			float theta = acos(2 * v - 1);
			GSOutput element;
			//element.position = input[0].position;
			float4 position = input[0].position;
			position.z = 50;
			element.position = position;//mul(position, worldViewProj);
			//element.position.xyz /= 10;
		//	element.position.x += 0.5;
			element.direction = normalize(float4(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi), 1));
			output.Append(element);
		}
	}
}