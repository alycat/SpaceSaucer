struct GSInput
{
	float4 position		 : SV_POSITION;
	float4 direction	: TEXCOORD;
};

cbuffer asteroidBuffer: register(b0)
{
	float age;
	float3 position;
	float3 acceleration;
};

float4 newPosition(GSInput input){
	return input.position + input.direction*age*0.75 + float4(0.5, 0, 0, 0)*age;
}

// The entry point for our vertex shader
GSInput main(GSInput vin)
{
	GSInput vout = vin;
	vout.position = newPosition(vout);
	return vout;
}