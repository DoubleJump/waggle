const static f32 E = 2.71828182845f;
const static f32 PI = 3.14159265359f;
const static f32 TAU = 6.28318530718f;
const static f32 SQRT_HALF = 0.7071067811865475f;
const static f32 EPSILON = 0.000001f;
const static f32 F32_MAX = 3.4028234e38f;
const static f32 DEG2RAD = PI / 180.0f;
const static f32 RAD2DEG = 57.5957795f;
const static f32 PI_OVER_360 = PI / 360.0f;
const static f32 PI_OVER_TWO = PI / 2.0f;
const static f32 PI_OVER_FOUR = PI / 4.0f;
const static f32 TWO_PI = 2.0f * PI;
const static f32 FOUR_PI = 4.0f * PI;
const static f32 GOLDEN_ANGLE = (PI*(3-sqrt(5.0f)));

static f32 
radians(f32 v)
{
	return v * DEG2RAD;
}

static f32 
degrees(f32 v)
{
	return v * RAD2DEG;
}

static u32
safe_truncate_u64(u64 value)
{
	assert(value <= UINT32_MAX, "Value is greater that UINT32_MAX");
	return (u32)value;
}

static f32
minimum(f32 a, f32 b) 
{ 
	if(a < b) return a; 
	return b; 
}

static f32
maximum(f32 a, f32 b) 
{ 
	if(a > b) return a; 
	return b; 
}

static i32
minimum(i32 a, i32 b) 
{ 
	if(a < b) return a;
	return b; 
}

static i32
maximum(i32 a, i32 b) 
{ 
	if(a > b) return a;
	return b; 
}

static f32 
clamp(f32 val, f32 min, f32 max)
{
	if(val < min) return min; 
	if(val > max) return max; 
	return val;
}

static i32 
clamp(i32 val, i32 min, i32 max)
{
	if(val < min) return min; 
	if(val > max) return max; 
	return val;
}

static i32
round_to_i32(f32 val)
{
	return (i32)roundf(val);
}

static u32
round_to_u32(f32 val)
{
	return (u32)roundf(val);
}

static i32
ceil_to_i32(f32 x)
{
	return (i32)ceilf(x);
}

static i32
floor_to_i32(f32 x)
{
	return (i32)floorf(x);
}

static memsize 
next_power_of_two(memsize v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

static f32 
snap_angle(f32 angle, f32 target)
{
	return floorf((fmod(angle, 360.0f) + target / 2.0f) / target) * target;
}

static f32
lerp(f32 a, f32 b, f32 t) 
{
	return (1-t) * a + t * b;
}

static f32 
move_towards(f32 val, f32 target, f32 rate)
{
	f32 result = val;
	if(target > val)
	{
		result += rate;
		if(result > target) return target;
	}
	else 
	{
		result -= rate;
		if(result < target) return target;
	}
	return result;
}

static f32 
smooth_move_towards(f32 val, f32 target, f32 rate, f32 epsilon)
{
	f32 result = val;
	f32 delta = target - val;
	if(fabs(delta) < epsilon) return target;
	result += (delta * 0.5f) * rate;
	return result;
}

static f32 
wrap_value(f32 value, f32 min, f32 max)
{
	if(value > max) return value - max;
	if(value < min) return value + max;
	return value;
}

static f32 
wrap_angle(f32 value)
{
	return wrap_value(value, 0.0f,360.0f);
}

static f32 
wrap_normal(f32 value)
{
	return wrap_value(value, 0.0f,1.0f);
}