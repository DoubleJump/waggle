static Triangle 
operator * (Triangle t, Mat4& m)
{
	return 
	{ 
		mul_point(t.a, m),
		mul_point(t.b, m), 
		mul_point(t.c, m)  
	};
}

static Vec3
get_normal(Triangle t)
{
	auto u = t.b - t.a;
	auto v = t.c - t.a;
	auto n = normalize(cross(u, v));
	return n;
}

static Vec3
get_center(Triangle t)
{
	return (t.a + t.b + t.c) / 3.0;
}

static Triangle
expand(Triangle t, f32 amount)
{
	Triangle r;
	auto center = get_center(t);
	r.a = t.a + (normalize(t.a - center) * amount);
	r.b = t.b + (normalize(t.b - center) * amount);
	r.c = t.c + (normalize(t.c - center) * amount);
	return r;
}

