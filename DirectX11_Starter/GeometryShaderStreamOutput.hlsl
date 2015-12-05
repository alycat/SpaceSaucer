struct Particle
{
	float4 position		: SV_POSITION;
	float4 speed		: COLOR;
};

cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

[maxvertexcount(50)]
void main(
	point Particle input[1],
	inout PointStream< Particle > output )
{
	/*float4 v[3];
	v[0] = input[0].position + float4(0.5f, -0.1f, 1.0f, 0);
	v[1] = input[0].position + float4(0.1f, 0.1f, -1.0f, 0);
	v[2] = input[0].position + float4(0.1f, -0.1f, 0.5f, 0);*/
	matrix worldViewProj = mul(mul(world, view), projection);
	for (uint i = 0; i < 50; i++)
	{
		Particle element = input[0];
		//element.position += float4(input[0].speed.z * 0.005* (i + 1), input[0].speed.w * 0.005* (i + 1), i, 0.0f);
		element.speed += float4((i+0.75)/10, (i + 0.75)/10, 0, 0);
		//element.position.z += (i+1)*0.005;
		//element.position = mul(element.position, worldViewProj);
		output.Append(element);
	}
//	output.Append(input[0]);
	
}