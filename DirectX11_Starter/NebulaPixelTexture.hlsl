struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD0;
	float2 uv			: TEXCOORD1;
	float age			: TEXCOORD2;
};

half noise(int x, int y){
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return saturate((1.0 - ((n * (n * n * 15731 + 789221) + 1376312589) & 2147483647) / 1073741824.0));
}

half gradientNoise(int x, int y){
	return pow(noise(x, y), 1.1);
}

half smoothedNoise(half x, half y){
	half corners = (noise(x - 1, y - 1) + noise(x + 1, y - 1) + noise(x - 1, y + 1) + noise(x + 1, y + 1)) / 16;
	half side = (noise(x - 1, y) + noise(x, y - 1) + noise(x + 1, y) + noise(x, y + 1)) / 8;
	half center = noise(x, y) / 4;
	return corners + side + center;
}

half interpolatedNoise(half x, half y){
	int _x = (int)x;
	half frac_x = x - _x;
	int _y = (int)y;
	half frac_y = y - _y;

	half v[4];
	v[0] = gradientNoise(x, y);
	v[1] = gradientNoise(x + 1, y);
	v[2] = gradientNoise(x, y + 1);
	v[3] = gradientNoise(x + 1, y + 1);

	half i1 = lerp(v[0], v[1], frac_x);
	half i2 = lerp(v[2], v[3], frac_x);

	return lerp(i1, i2, frac_y);
}

half4 octave(half x, half y, half pers, int octave){
	half freq = 1.0f;
	half amp = 1.0f;
	half max = 0.0f;
	half total = 0.0f;
	for (int i = 0; i < octave; ++i){
		total += interpolatedNoise(x * freq, y * freq) * amp;
		max += amp;
		amp = pow(pers, (half) i );
		freq = 2 ^ i;
	}
	//total /= max;
	return saturate(total);
}

half3 color(half t){
	half r = min(abs(sin(t * 6)), t);
	half g = t - min(abs(sin(t * 5.5)), t);
	return half3(r, g, t);
}

float4 main(VertexToPixel input) : SV_TARGET
{
	int oct = 3;
	//oct = 1;
	half pers = 0.5;
	half x = input.position.x;
	half y = input.position.y;
	half u = input.uv.x - 0.5;
	half v = input.uv.y - 0.5;
	half opacity = 0.25 - ((u * u) + ( v * v));
	half freq = (1.0/(20.0/opacity));
	half age = input.age;
	half off = age/2;
	half newX = x*freq+off;
	half newY = y*freq+off;
	half tone = octave((x* freq) + off, (y* freq) + off, pers, oct);
	tone *= opacity * 2.5;
	opacity = lerp(opacity, tone, 0.5);
	half4 neb = half4(color(clamp(tone, 0, 1)), opacity);
	half n = (interpolatedNoise(x*freq + age, y*freq + age)/3);
	half s = smoothedNoise((x*(0.5/opacity)) + ((age*1) + (2.45 * age * min(age, 5))), (y*(0.5/opacity)) - ((age*2) + (4.9 * age * min(age, 5))));

	neb += half4(n, n, n, n);
	if (s >= 0.65){
		neb += half4(s, s, s*0.5, s)*0.5;
	}
	return saturate(neb);
}