static Vec3 
operator - (Vec3 a)
{
	return { -a.x, -a.y, -a.z };
}

static Vec3 
operator + (Vec3 a, Vec3 b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

static Vec3 
operator + (Vec3 a, Vec2 b)
{
	return { a.x + b.x, a.y + b.y};
}

static Vec3 
operator + (Vec2 a, Vec3 b)
{
	return { a.x + b.x, a.y + b.y};
}

static Vec3 
operator + (Vec3 a, f32 b)
{
	return { a.x + b, a.y + b, a.z + b };
}

static Vec3 
operator - (Vec3 a, Vec3 b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

static Vec3 
operator - (Vec3 a, f32 b)
{
	return { a.x - b, a.y - b, a.z - b };
}

static Vec3 
operator * (Vec3 a, f32 f)
{
	return { a.x * f, a.y * f, a.z * f };
}

static Vec3 
operator * (f32 f, Vec3 a)
{
	return { a.x * f, a.y * f, a.z * f };
}

static Vec3 
operator / (Vec3 a, f32 f)
{
	return { a.x / f, a.y / f, a.z / f };
}

static Vec3 
operator / (f32 f, Vec3 a)
{
	return { f / a.x, f / a.y, f / a.z };
}

static Vec3 
operator += (Vec3& a, Vec3 b)
{
	a = a + b;
	return a;
}

static Vec3 
operator -= (Vec3& a, Vec3 b)
{
	a = a - b;
	return a;
}

static Vec3 
operator += (Vec3& a, Vec2 b)
{
	a.x = a.x + b.x;
	a.y = a.y + b.y;
	return a;
}

static Vec3 
operator -= (Vec3& a, Vec2 b)
{
	a.x = a.x - b.x;
	a.y = a.y - b.y;
	return a;
}

static Vec3 
operator *= (Vec3& a, f32 f)
{
	a = a * f;
	return a;
}

static Vec3 
operator /= (Vec3& a, f32 f)
{
	a = a / f;
	return a;
}

static Vec3
to_vec3(Vec2 v)
{
	return Vec3{v.x, v.y, 0};
}

static b32
operator == (Vec3 a, Vec3 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

static b32 
approx_equal(Vec3 a, Vec3 b, f32 epsilon)
{
	for(int i = 0; i < 3; ++i)
	{
		if(fabs( a[i] - b[i]) > epsilon) return false;
	}
	return true;
}

static Vec3
minimum(Vec3 a, Vec3 b)
{
	return 
	{
		minimum(a.x, b.x),
		minimum(a.y, b.y),
		minimum(a.z, b.z)
	};
}

static Vec3
maximum(Vec3 a, Vec3 b)
{
	return 
	{
		maximum(a.x, b.x),
		maximum(a.y, b.y),
		maximum(a.z, b.z)
	};
}

static f32
dot(Vec3 a, Vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static f32
sqr_length(Vec3 a)
{
	return dot(a, a);
}

static f32 
length(Vec3 v) 
{
	return sqrt(sqr_length(v));
}

static f32 
sqr_distance(Vec3 a, Vec3 b)
{
	return sqr_length(b-a);
}

static f32 
distance(Vec3 a, Vec3 b)
{
	return sqrt(sqr_distance(a,b));
}

static Vec3 
normalize(Vec3 v)
{
	f32 l = length(v);
	if(l > EPSILON) return v / l;
	return {0,0,0};
}

static Vec3
clamp_length(Vec3 v, f32 l)
{
	if(sqr_length(v) > l * l) return normalize(v) * l;
	return v;
}

static Vec3 
reflect(Vec3 a, Vec3 n)
{
	return -2.0f * dot(n,a) * n + a;
}

static Vec3 
cross(Vec3 a, Vec3 b)
{
	return 
	{
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

static Vec3
project(Vec3 a, Vec3 b)
{
	f32 d = dot(b,b);
	if(d < EPSILON) return {0,0,0};
	return b * dot(a,b) / d;
}

/*
static Vec3
tangent(Vec3 a, Vec3 b, Vec3 plane)
{
	return cross(normalize(a + b), plane);
}
*/

static f32
angle_between(Vec3 a, Vec3 b)
{
	f32 l = length(a) * length(b);
	if(l < EPSILON) l = EPSILON;
	f32 f = dot(a,b) / l;

	if(f > 1.0f) return acosf(1.0f);
	else if(f < 1.0f) return acosf(-1.0f);
	else return acosf(f);
}

static Vec3
rotate_2D(Vec3 v, f32 angle)
{
	f32 ar = radians(angle);
	f32 cr = cos(ar);
	f32 sr = sin(ar);
	return 
	{
		v.x * cr - v.y * sr,
		v.x * sr + v.y * cr,
		v.z
	};
}

static Vec3
ortho_normalize(Vec3 base, Vec3 v)
{
	return normalize(cross(base, v));
}

static Vec3
lerp(Vec3 a, Vec3 b, f32 t)
{
	Vec3 r;
	r.x = lerp(a.x, b.x, t);
	r.y = lerp(a.y, b.y, t);
	r.z = lerp(a.z, b.z, t);
	return r;
}

