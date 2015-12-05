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

outParticle main(inParticle input)
{
	outParticle output = input;
	return output;
}