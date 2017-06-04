static Vec4 
operator * (Vec4 a, Vec4 b)
{
	return
	{
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
	};
}

static Vec4 
operator *= (Vec4& a, Vec4 b)
{
	a = a * b;
	return a;
}

static Vec3 
operator * (Vec4 q, Vec3 v)
{
	f32 tx = (q.y * v.z - q.z * v.y) * 2.0f;
	f32 ty = (q.z * v.x - q.x * v.z) * 2.0f;
	f32 tz = (q.x * v.y - q.y * v.x) * 2.0f;

	f32 cx = q.y * tz - q.z * ty;
	f32 cy = q.z * tx - q.x * tz;
	f32 cz = q.x * ty - q.y * tx;

	return
	{
		v.x + q.w * tx + cx,
		v.y + q.w * ty + cy,
		v.z + q.w * tz + cz
	};
}

static Vec4
quaternion_identity()
{
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

static Vec4 
conjugate(Vec4 q) 
{
	return { -q.x, -q.y, -q.z, q.w };
}

static Vec4 
inverse(Vec4 q)
{
	return normalize(conjugate(q));
}

static Vec4 
rotation_from_euler(Vec3 v)
{
	f32 xr = (v.x * DEG2RAD) / 2.0f;
	f32 yr = (v.y * DEG2RAD) / 2.0f;
	f32 zr = (v.z * DEG2RAD) / 2.0f;

	f32 sx = sinf(xr);
	f32 sy = sinf(yr);
	f32 sz = sinf(zr);
	f32 cx = cosf(xr);
	f32 cy = cosf(yr);
	f32 cz = cosf(zr);

	return
	{
		sx * cy * cz - cx * sy * sz,
		cx * sy * cz + sx * cy * sz,
		cx * cy * sz - sx * sy * cz,
		cx * cy * cz + sx * sy * sz
	};
}

static Vec3 
get_euler(Vec4 q)
{
	Vec3 r;

    f32 sqx = q.x * q.x;
    f32 sqy = q.y * q.y;
    f32 sqz = q.z * q.z;
    f32 sqw = q.w * q.w;

	f32 unit = sqx + sqy + sqz + sqw;
	f32 test = q.x * q.y + q.z * q.w;
	f32 TOLERANCE = 0.499f;

	if(test > TOLERANCE * unit) 
	{
		r.x = 0.0f;
		r.y = 2.0f * atan2f(q.x, q.w);
		r.z = PI_OVER_TWO;
	}
	else if(test < -TOLERANCE * unit) 
	{ 
		r.x = 0.0f;
		r.y = -2.0f * atan2f(q.x, q.w);
		r.z = -PI_OVER_TWO;
	}
	else
	{
		f32 d = sqy - sqz + sqw;
		r.x = atan2f(2.0f * q.x * q.w - 2.0f * q.y * q.z, -sqx + d);
		r.y = atan2f(2.0f * q.y * q.w - 2.0f * q.x * q.z, sqx - d);
		r.z = asinf(2.0f * test / unit);
	}
   
	return r * RAD2DEG;
}

static Vec4
rotation_from_angle_axis(f32 angle, Vec3 axis)
{
	f32 h = 0.5f * radians(angle);
	f32 s = sinf(h);
	return 
	{	
		s * axis.x,
		s * axis.y,
		s * axis.z,
		cosf(h)
	};
}

static Vec4 
get_angle_axis(Vec4 q)
{
	Vec4 r;
	f32 l = sqr_length(q);
	if(l > EPSILON)
	{
		f32 i = 1.0f / sqrtf(l);
		r.x = q.x * i;
		r.y = q.y * i;
		r.z = q.z * i;
		r.w = (2.0f * acosf(q.w)) * RAD2DEG;
	}
	else
	{
		r.x = q.x;
		r.y = q.y;
		r.z = q.z;
		r.w = 0.0f;
	}
	return r;
}

static Vec4 
from_to_rotation(Vec3 from, Vec3 to)
{
	Vec3 f = normalize(from);
	Vec3 t = normalize(to);
	Vec3 c = cross(f, t);
	Vec4 r = 
	{
		c.x,
		c.y,
		c.z,
		1.0f + dot(f, t)
	};
	return normalize(r);
}

static Vec4 
look_at_rotation(Vec3 from, Vec3 to, Vec3 forward)
{
	return from_to_rotation(forward, from-to);
}

static Vec4 
slerp(Vec4 a, Vec4 b, f32 t) 
{
	Vec4 r;
	f32 flip = 1;
	f32 cosine = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
	
	if(cosine < 0) 
	{ 
		cosine = -cosine; 
		flip = -1;
	} 
	if((1 - cosine) < EPSILON)
	{
		r = lerp(a,b,t * flip);
		return normalize(r);
	}
	
	f32 theta = acosf(cosine); 
	f32 sine = sinf(theta); 
	f32 beta = sinf((1.0 - t) * theta) / sine; 
	f32 alpha = sinf(t * theta) / sine * flip;
	
	r = a * beta + b * alpha;
	return normalize(r);
}