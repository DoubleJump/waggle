static f32 
rand_f32(f32 min, f32 max)
{
	f32 random = ((f32)rand()/(f32)(RAND_MAX));
	f32 delta = max - min;
	f32 r = random * delta;
	return min + r;
}

static f32 
rand_sign()
{
	f32 random = rand_f32(0.0f, 1.0f);
	if(random > 0.5) return 1.0f;
	return -1.0f;
}

static Vec3 
rand_unit_circle(f32 radius = 1.0)
{
	f32 x = rand_f32(-1.0, 1.0);
	f32 y = rand_f32(-1.0, 1.0);
	f32 l = (1.0 / sqrt(x * x + y * y)) * rand_f32(1.0, radius);
	return { x*l, y*l, 0.0 };
}

static Vec3
rand_vec3(Vec3 min, Vec3 max)
{
	Vec3 result;
	for(auto i = 0; i < 3; ++i)
		result[i] = rand_f32(min[i], max[i]);

	return result;
}

static Vec4
rand_vec4(Vec4 min, Vec4 max)
{
	Vec4 result;
	for(auto i = 0; i < 4; ++i)
		result[i] = rand_f32(min[i], max[i]);

	return result;
}