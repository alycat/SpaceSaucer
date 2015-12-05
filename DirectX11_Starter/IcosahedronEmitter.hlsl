struct GSOutput
{
	float4 position : SV_POSITION;
	float4 direction : TEXCOORD;
};


static float phi = 2 * sqrt(5);

static float3 ico[12] = {
	phi, 1, 0,
	-phi, 1, 0,
	-phi, -1, 0,
	phi, -1, 0,

	0, -phi, 1,
	0, phi, 1,
	1, 0, phi,
	-1, 0, phi,

	-1, 0, -phi,
	1, 0, -phi,
	0, -phi, -1,
	0, phi, -1,

};


[maxvertexcount(120)]
void main(
	point GSOutput input[1], 
	inout PointStream< GSOutput > output
)
{
	float r = 0.3;
	for (uint i = 0; i < 12; i++)
	{
		GSOutput element;
		element.position = input[0].position + float4(ico[i], 0) * r;
		element.direction = normalize(float4(-ico[i], 0));
		output.Append(element);
	}
}