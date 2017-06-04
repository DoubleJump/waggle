static Vec4 
operator - (Vec4 a)
{
	return { -a.x, -a.y, -a.z, -a.w };
}

static Vec4 
operator + (Vec4 a, Vec4 b)
{
	return { a.x + b.x, a.y + b.y, a.z + b.z, a.w * b.w };
}

static Vec4 
operator + (Vec4 a, f32 b)
{
	return { a.x + b, a.y + b, a.z + b, a.w + b };
}

static Vec4 
operator - (Vec4 a, Vec4 b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

static Vec4 
operator - (Vec4 a, f32 b)
{
	return { a.x - b, a.y - b, a.z - b, a.w - b};
}

static Vec4 
operator * (Vec4 a, f32 f)
{
	return { a.x * f, a.y * f, a.z * f, a.w * f };
}

static Vec4 
operator * (f32 f, Vec4 a)
{
	return { a.x * f, a.y * f, a.z * f, a.w * f};
}

static Vec4 
operator / (Vec4 a, f32 f)
{
	return { a.x / f, a.y / f, a.z / f, a.w / f };
}

static Vec4 
operator += (Vec4& a, Vec4 b)
{
	a = a + b;
	return a;
}

static Vec4 
operator -= (Vec4& a, Vec4 b)
{
	a = a - b;
	return a;
}

static Vec4 
operator *= (Vec4& a, f32 f)
{
	a = a * f;
	return a;
}

static Vec4 
operator /= (Vec4& a, f32 f)
{
	a = a / f;
	return a;
}

static b32
operator == (Vec4 a, Vec4 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

static b32 
approx_equal(Vec4 a, Vec4 b, f32 epsilon)
{
	for(int i = 0; i < 4; ++i)
	{
		if(fabs(a[i] - b[i]) > epsilon) return false;
	}
	return true;
}

static f32
dot(Vec4 a, Vec4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static f32
sqr_length(Vec4 a)
{
	return dot(a, a);
}

static f32 
length(Vec4 v) 
{
	return sqrt(sqr_length(v));
}

static f32 
sqr_distance(Vec4 a, Vec4 b)
{
	return sqr_length(b-a);
}

static f32 
distance(Vec4 a, Vec4 b)
{
	return sqrt(sqr_distance(a,b));
}

static Vec4 
normalize(Vec4 v)
{
	f32 l = sqr_length(v);
	if(l > EPSILON) return v * sqrt(1.0f / l);
	return v;
}

static Vec4
rectangle_from_width_height(Vec3 p, f32 w, f32 h)
{
	auto hw = w / 2;
	auto hh = h / 2;
	return {p.x - hw, p.y + hh, w,h};
}

static Vec4
lerp(Vec4 a, Vec4 b, f32 t)
{
	Vec4 r;
	r.x = lerp(a.x, b.x, t);
	r.y = lerp(a.y, b.y, t);
	r.z = lerp(a.z, b.z, t);
	r.w = lerp(a.w, b.w, t);
	return r;
}