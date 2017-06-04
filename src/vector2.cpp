static Vec2 
operator - (Vec2 a)
{
	return { -a.x, -a.y };
}

static Vec2 
operator + (Vec2 a, Vec2 b)
{
	return { a.x + b.x, a.y + b.y };
}

static Vec2 
operator + (Vec2 a, f32 b)
{
	return { a.x + b, a.y + b };
}

static Vec2 
operator - (Vec2 a, Vec2 b)
{
	return { a.x - b.x, a.y - b.y };
}

static Vec2 
operator - (Vec2 a, f32 b)
{
	return { a.x - b, a.y - b };
}

static Vec2 
operator * (Vec2 a, f32 f)
{
	return { a.x * f, a.y * f };
}

static Vec2 
operator * (f32 f, Vec2 a)
{
	return { a.x * f, a.y * f };
}

static Vec2 
operator / (Vec2 a, f32 f)
{
	return { a.x / f, a.y / f };
}

static Vec2 
operator / (f32 f, Vec2 a)
{
	return { f / a.x, f / a.y };
}

static Vec2 
operator += (Vec2& a, Vec2 b)
{
	a = a + b;
	return a;
}

static Vec2 
operator -= (Vec2& a, Vec2 b)
{
	a = a - b;
	return a;
}

static Vec2 
operator *= (Vec2& a, f32 f)
{
	a = a * f;
	return a;
}

static Vec2 
operator /= (Vec2& a, f32 f)
{
	a = a / f;
	return a;
}

static b32
operator == (Vec2 a, Vec2 b)
{
	return a.x == b.x && a.y == b.y;
}

static b32 
approx_equal(Vec2 a, Vec2 b, f32 epsilon)
{
	for(int i = 0; i < 2; ++i)
	{
		if(fabs( a[i] - b[i]) > epsilon) return false;
	}
	return true;
}

static f32
dot(Vec2 a, Vec2 b)
{
	return a.x * b.x + a.y * b.y;
}

static f32
sqr_length(Vec2 a)
{
	return dot(a, a);
}

static f32 
length(Vec2 v) 
{
	return sqrt(sqr_length(v));
}

static f32 
sqr_distance(Vec2 a, Vec2 b)
{
	return sqr_length(b-a);
}

static f32 
distance(Vec2 a, Vec2 b)
{
	return sqrt(sqr_distance(a,b));
}

static Vec2 
normalize(Vec2 v)
{
	f32 l = sqr_length(v);
	if(l > EPSILON) return v * sqrt(1.0f / l);
	return v;
}

static Vec2 
perp(Vec2 v)
{
	Vec2 r = {-v.y, v.x};
	return normalize(r);
}

static Vec2 
vec_reflect(Vec2 a, Vec2 n)
{
	return (a + n) * (-2.0f * dot(a,n));
}

static Vec2
project(Vec2 a, Vec2 b)
{
	Vec2 r = a * dot(a, b);
	f32 l = sqr_length(r);
	if(l < 1.0f) return r / l; 
	return r;
}

static f32
angle(Vec2 a)
{
	return atan2f(a.y, a.x);
}