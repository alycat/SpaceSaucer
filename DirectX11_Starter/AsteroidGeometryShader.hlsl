cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct GSInput
{
	float4 position : SV_POSITION;
	float4 direction: TEXCOORD;
};

struct GSOutput
{

	float4 position		 : SV_POSITION;
	float3 normal		 : NORMAL;
	float2 uv		     : TEXCOORD0;
	float3 tangent		 : TEXCOORD1;
	float4 direction	 : TEXCOORD2;
};



void createParticle(GSOutput input, inout TriangleStream<GSOutput> output, float3 direction){

	//matrix worldViewProj = mul(mul(world, view), projection);
	float d = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
	float4 v[6];
	float4 position = input.position;
	float r = 1;

	float3 perpendicular = normalize(cross(direction, -direction));
	float3 perpendicular2 = normalize(cross(direction, perpendicular));

	v[0] = position + float4(direction, 0)*r*2;
	v[1] = position + float4(perpendicular,0)*r;
	v[2] = position + float4(perpendicular2,0)*r;
	v[3] = position - float4(perpendicular,0)*r;
	v[4] = position - float4(perpendicular2,0)*r;
	v[5] = position - float4(direction, 0)*r*0.5;

	float2 quadUVs[6] = {
		float2(0.5, 1),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(0.5, 0),
	};
	
	float3 tangents[6];
	for (int i = 0; i < 6; ++i){
		float x = v[i].x;
		float y = v[i].y;
		float z = v[i].z;
		float r = sqrt(x*x + y*y + z*z);
		float theta = acos(z / r);
		float phi = atan2(y, x) +(3.141529 / 2);
		tangents[i].x = sin(theta)*cos(phi);
		tangents[i].y = sin(theta)*sin(phi);
		tangents[i].z = cos(theta);
	}
	/*
	float3 tangents[6] = {
		normalize(v[1] - v[0]).xyz,
		normalize(v[2] - v[1]).xyz,
		normalize(v[3] - v[2]).xyz,
		normalize(v[4] - v[3]).xyz,
		normalize(v[1] - v[5]).xyz,
		normalize(v[4] - v[5]).xyz,
	};*/
	
	float3 normals[6] = {
		direction,
		perpendicular,
		perpendicular2,
		-perpendicular,
		-perpendicular2,
		-direction
	};
	/*
	float3 tangents[6] = {
		-perpendicular,
		-perpendicular2,
		-direction,
		perpendicular2,
		direction,
		perpendicular,
	};*/
	

	float indices[24] = { 
		1, 4, 0,
		5, 4,1, 

		2, 0, 1,
		5,1, 2, 

		3, 0, 2,
		5, 2, 3,

		4, 0, 3,
		5, 3, 4
	};

	matrix worldViewProj = mul(mul(world, view), projection);

	GSOutput element = input;
	for (uint i = 0; i < 24; i++)
	{
		element.position = mul(v[indices[i]], worldViewProj);
		element.uv = quadUVs[indices[i]];
		element.normal = normals[indices[i]];
		element.tangent = tangents[indices[i]];
		output.Append(element);
	}
	output.RestartStrip();
}


[maxvertexcount(24)]
void main(
	point GSInput input[1], 
	inout TriangleStream< GSOutput > output
)
{
	GSOutput element;
	element.position = input[0].position;
	element.normal = float3(0, 0, 0);
	element.tangent = float3(0, 0, 1);
	element.uv = float2(0, 0);
	element.direction = input[0].direction;
	createParticle(element, output, input[0].direction);
}