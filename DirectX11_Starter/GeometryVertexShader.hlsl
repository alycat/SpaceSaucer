struct inParticle
{
	float4 position		: POSITION;
	float4 speed		: COLOR;
};

struct outParticle
{
	float4 position		: SV_POSITION;
	float4 speed		: COLOR;
};

cbuffer geoBuffer : register(b0)
{
	float age;
};

float calculateUnit(float velocity, float acceleration, float position){
	return ((0.5f * age * age * acceleration) + (age * velocity) + position);
}

float4 calculatePosition(float2 velocity, float2 acceleration, float4 position)
{
	//return float4(0.5 * position.x * age * cos(age), 0.5 * position.y * age * sin(age), position.z, position.w);
	return float4(calculateUnit(velocity.x, acceleration.x, position.x * age * cos(age)), calculateUnit(velocity.y, acceleration.y, position.y * age * sin(age)), position.z, position.w);
}

outParticle main(inParticle input)
{
	outParticle output;
	output.speed = input.speed;
//	output.position = float4((input.speed.x * input.position.x * age * cos(age)), (input.speed.y * input.position.y * age * sin(age)), input.position.z, input.position.w);
	output.position = float4(( (cos(age)/( input.speed.x * age))) + input.position.x, (( sin(age)/( input.speed.y * age))) + input.position.y, input.position.z, input.position.w);
	//output.position = input.position;
	float2 velocity = float2(0, 0);
	float2 acceleration = float2(0, 0);
	//output.position = calculatePosition(velocity, acceleration, pos);
	return output;
}