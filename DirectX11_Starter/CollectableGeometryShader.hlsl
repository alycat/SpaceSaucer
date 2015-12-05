struct GSInput
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD0;
	float2 uv			: TEXCOORD1;
	float age			: TEXCOORD2;
};

struct GSOutput
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD0;
	float2 uv			: TEXCOORD1;
	float2 pos			: TEXCOORD2;
	float age : TEXCOORD3;
};

cbuffer perModel		: register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};


static float angle = 0.62831853072;
static float halfPi = 1.5707963268;
static float r = 2.0f;

static float4 star[] = {
	{ 0, 0, 1 , 1},

	{ cos(angle) * r, sin(angle) * r, 2 , 1},
	{ cos(2 * angle) * 0.5 * r, sin(angle * 2) * 0.5 * r, 2, 1 },
	{ cos(3 * angle) * r, sin(3 * angle) * r, 2 , 1},
	{ cos(4 * angle) * 0.5 * r, sin(4 * angle) * 0.5 * r, 2 , 1},
	{ cos(5 * angle) * r, sin(5 * angle) * r, 2 , 1},

	{ cos(6 * angle) * 0.5 * r, sin(6 * angle) * 0.5 * r, 2 , 1},
	{ cos(7 * angle) * r, sin(7 * angle) * r, 2 , 1},
	{ cos(8 * angle) * 0.5 * r, sin(8 * angle) * 0.5 * r, 2 , 1},
	{ cos(9 * angle) * r, sin(9 * angle) * r, 2 , 1},
	{ cos(10 * angle) * 0.5 * r, sin(10 * angle) * 0.5 * r, 2 , 1},

	{ 0, 0, 3, 1 },
};

static float2 uvs[] = {
	{ 0.5, 0.5 },

	{ (1 + cos(angle)) / 2, (1 + sin(angle)) / 2 },
	{ (1 + (cos(2 * angle) * 0.5)) / 2, (1 + (sin(angle * 2) * 0.5)) / 2 },
	{ (1 + cos(3 * angle)) / 2, (1 + sin(3 * angle)) / 2 },
	{ (1 + (cos(4 * angle) * 0.5)) / 2, (1 + (sin(4 * angle) * 0.5)) / 2 },
	{ (1 + cos(5 * angle)) / 2, (1 + sin(5 * angle)) / 2 },

	{ (1 + (cos(6 * angle) * 0.5)) / 2, (1 + (sin(6 * angle) * 0.5)) / 2 },
	{ (1 + cos(7 * angle)) / 2, (1 + sin(7 * angle)) / 2 },
	{ (1 + (cos(8 * angle) * 0.5)) / 2, (1 + (sin(8 * angle) * 0.5)) / 2 },
	{ (1 + cos(9 * angle)) / 2, (1 + sin(9 * angle)) / 2 },
	{ (1 + (cos(10 * angle) * 0.5)) / 2, (1 + (sin(10 * angle) * 0.5)) / 2 },

	{ 0.5, 0.5 }
};

static float3 vertexNormals[] = {
	{ 0, 0, 1 },

	{ cos(angle), sin(angle), 0 },
	{ cos(2 * angle), sin(angle * 2), 0 },
	{ cos(3 * angle), sin(3 * angle), 0 },
	{ cos(4 * angle), sin(4 * angle), 0 },
	{ cos(5 * angle), sin(5 * angle), 0 },

	{ cos(6 * angle), sin(6 * angle), 0 },
	{ cos(7 * angle), sin(7 * angle), 0 },
	{ cos(8 * angle), sin(8 * angle), 0 },
	{ cos(9 * angle), sin(9 * angle), 0 },
	{ cos(10 * angle), sin(10 * angle), 0 },

	{ 0, 0, -1 },
};

static float3 surfaceNormals[] = {
	normalize(cross(star[2] - star[1], star[2] - star[0])),
	normalize(cross(star[2] - star[3], star[2] - star[0])),
	normalize(cross(star[0] - star[4], star[0] - star[3])),
	normalize(cross(star[5] - star[0], star[2] - star[4])),
	normalize(cross(star[6] - star[0], star[6] - star[5])),

	normalize(cross(star[7] - star[6], star[7] - star[0])),
	normalize(cross(star[8] - star[0], star[8] - star[7])),
	normalize(cross(star[0] - star[8], star[0] - star[9])),
	normalize(cross(star[0] - star[10], star[0] - star[9])),
	normalize(cross(star[0] - star[10], star[0] - star[1])),

	normalize(cross(star[2] - star[1], star[2] - star[11])),
	normalize(cross(star[2] - star[3], star[2] - star[0])),
	normalize(cross(star[11] - star[4], star[11] - star[3])),
	normalize(cross(star[5] - star[11], star[2] - star[4])),
	normalize(cross(star[6] - star[11], star[6] - star[5])),

	normalize(cross(star[7] - star[6], star[7] - star[11])),
	normalize(cross(star[8] - star[11], star[8] - star[7])),
	normalize(cross(star[11] - star[8], star[11] - star[9])),
	normalize(cross(star[11] - star[10], star[11] - star[9])),
	normalize(cross(star[11] - star[10], star[11] - star[1]))
};


static int indices[] = {
	1, 0, 2,
	0, 2, 3,
	4, 3, 0, 
	0, 4, 5,
	5, 0, 6,

	0, 6, 7,
	7, 0, 8,
	9, 8, 0,
	9, 10, 0,
	1, 10,0, 


	11, 2, 1,
	11, 2, 3,
	11, 3, 4,
	11, 4, 5,
	11, 5, 6,

	11, 6, 7,
	11, 7, 8,
	11, 8, 9,
	11, 9, 10,
	11, 10, 1,
};




[maxvertexcount(60)]
void main(
	point GSInput input[1],
	inout TriangleStream< GSOutput > output
)
{
	matrix worldViewProj = mul(mul(world, view), projection);
/*	
	float4 v[4];
	float4 temp = float4(input[0].position);
	v[0] = temp + float4(r, -r, 0.0f, 0.0f);
	v[1] = temp + float4(-r, -r, 0.0f, 0.0f);
	v[2] = temp + float4(r, r, 0.0f, 0.0f);
	v[3] = temp + float4(-r, r, 0.0f, 0.0f);


	float2 quadUVs[4] = {
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f)
	};

	GSOutput element;
	[unroll]
	for (uint i = 0; i < 4; i++)
	{
		element.position = mul(v[i], worldViewProj);
		element.uv = quadUVs[i];
		element.normal = input[0].normal;
		element.age = input[0].age;
		output.Append(element);
	}
	output.RestartStrip();
	*/

	
	for (uint i = 0; i < 60; i++){
		GSOutput element;// = input[0];
		element.position = mul(star[indices[i]], worldViewProj);
		element.normal = normalize(mul(vertexNormals[indices[i]], (float3x3)world)); // vertexNormals[indices[i]];
		element.uv = uvs[indices[i]];
		element.age = input[0].age;
		element.pos = input[0].position.xy;
		output.Append(element);
	}
}