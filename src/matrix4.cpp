static Mat4
mat4_identity()
{
	return
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};

static Mat4 
operator * (Mat4 a, Mat4 b)
{
	Mat4 r;

	r[ 0] = a[ 0] * b[0] + a[ 1] * b[4] + a[ 2] * b[ 8] + a[ 3] * b[12];
	r[ 1] = a[ 0] * b[1] + a[ 1] * b[5] + a[ 2] * b[ 9] + a[ 3] * b[13];
	r[ 2] = a[ 0] * b[2] + a[ 1] * b[6] + a[ 2] * b[10] + a[ 3] * b[14];
	r[ 3] = a[ 0] * b[3] + a[ 1] * b[7] + a[ 2] * b[11] + a[ 3] * b[15];
	r[ 4] = a[ 4] * b[0] + a[ 5] * b[4] + a[ 6] * b[ 8] + a[ 7] * b[12];
	r[ 5] = a[ 4] * b[1] + a[ 5] * b[5] + a[ 6] * b[ 9] + a[ 7] * b[13];
	r[ 6] = a[ 4] * b[2] + a[ 5] * b[6] + a[ 6] * b[10] + a[ 7] * b[14];
	r[ 7] = a[ 4] * b[3] + a[ 5] * b[7] + a[ 6] * b[11] + a[ 7] * b[15];	
	r[ 8] = a[ 8] * b[0] + a[ 9] * b[4] + a[10] * b[ 8] + a[11] * b[12];
	r[ 9] = a[ 8] * b[1] + a[ 9] * b[5] + a[10] * b[ 9] + a[11] * b[13];
	r[10] = a[ 8] * b[2] + a[ 9] * b[6] + a[10] * b[10] + a[11] * b[14];
	r[11] = a[ 8] * b[3] + a[ 9] * b[7] + a[10] * b[11] + a[11] * b[15];
	r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[ 8] + a[15] * b[12];
	r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[ 9] + a[15] * b[13];
	r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
	r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];

	return r;
}

static Mat4 
operator *= (Mat4& a, Mat4 b)
{
	a = a * b;
	return a;
}

static Mat4 
operator - (Mat4 m)
{
	Mat4 r;

	f32 v0 = m[ 2] * m[ 7] - m[ 6] * m[ 3];
	f32 v1 = m[ 2] * m[11] - m[10] * m[ 3];
	f32 v2 = m[ 2] * m[15] - m[14] * m[ 3];
	f32 v3 = m[ 6] * m[11] - m[10] * m[ 7];
	f32 v4 = m[ 6] * m[15] - m[14] * m[ 7];
	f32 v5 = m[10] * m[15] - m[14] * m[11];

	f32 t0 =   v5 * m[5] - v4 * m[9] + v3 * m[13];
	f32 t1 = -(v5 * m[1] - v2 * m[9] + v1 * m[13]);
	f32 t2 =   v4 * m[1] - v2 * m[5] + v0 * m[13];
	f32 t3 = -(v3 * m[1] - v1 * m[5] + v0 * m[ 9]);

	f32 idet = 1.0f / (t0 * m[0] + t1 * m[4] + t2 * m[8] + t3 * m[12]);

	r[0] = t0 * idet;
	r[1] = t1 * idet;
	r[2] = t2 * idet;
	r[3] = t3 * idet;

	r[4] = -(v5 * m[4] - v4 * m[8] + v3 * m[12]) * idet;
	r[5] =  (v5 * m[0] - v2 * m[8] + v1 * m[12]) * idet;
	r[6] = -(v4 * m[0] - v2 * m[4] + v0 * m[12]) * idet;
	r[7] =  (v3 * m[0] - v1 * m[4] + v0 * m[ 8]) * idet;

	v0 = m[1] * m[ 7] - m[ 5] * m[3];
	v1 = m[1] * m[11] - m[ 9] * m[3];
	v2 = m[1] * m[15] - m[13] * m[3];
	v3 = m[5] * m[11] - m[ 9] * m[7];
	v4 = m[5] * m[15] - m[13] * m[7];
	v5 = m[9] * m[15] - m[13] * m[11];

	r[ 8] =  (v5 * m[4] - v4 * m[8] + v3 * m[12]) * idet;
	r[ 9] = -(v5 * m[0] - v2 * m[8] + v1 * m[12]) * idet;
	r[10] =  (v4 * m[0] - v2 * m[4] + v0 * m[12]) * idet;
	r[11] = -(v3 * m[0] - v1 * m[4] + v0 * m[ 8]) * idet;

	v0 = m[ 6] * m[1] - m[ 2] * m[ 5];
	v1 = m[10] * m[1] - m[ 2] * m[ 9];
	v2 = m[14] * m[1] - m[ 2] * m[13];
	v3 = m[10] * m[5] - m[ 6] * m[ 9];
	v4 = m[14] * m[5] - m[ 6] * m[13];
	v5 = m[14] * m[9] - m[10] * m[13];

	r[12] = -(v5 * m[4] - v4 * m[8] + v3 * m[12]) * idet;
	r[13] =  (v5 * m[0] - v2 * m[8] + v1 * m[12]) * idet;
	r[14] = -(v4 * m[0] - v2 * m[4] + v0 * m[12]) * idet;
	r[15] =  (v3 * m[0] - v1 * m[4] + v0 * m[ 8]) * idet;

	return r;
}

static f32
determinant(Mat4 m)
{
	f32 a0 = m[ 0] * m[ 5] - m[ 1] * m[ 4];
	f32 a1 = m[ 0] * m[ 6] - m[ 2] * m[ 4];
	f32 a2 = m[ 0] * m[ 7] - m[ 3] * m[ 4];
	f32 a3 = m[ 1] * m[ 6] - m[ 2] * m[ 5];
	f32 a4 = m[ 1] * m[ 7] - m[ 3] * m[ 5];
	f32 a5 = m[ 2] * m[ 7] - m[ 3] * m[ 6];
	f32 b0 = m[ 8] * m[13] - m[ 9] * m[12];
	f32 b1 = m[ 8] * m[14] - m[10] * m[12];
	f32 b2 = m[ 8] * m[15] - m[11] * m[12];
	f32 b3 = m[ 9] * m[14] - m[10] * m[13];
	f32 b4 = m[ 9] * m[15] - m[11] * m[13];
	f32 b5 = m[10] * m[15] - m[11] * m[14];

	return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
}

static Mat4
transpose(Mat4 m)
{
	Mat4 r;

	r[ 1] = m[ 4]; 
	r[ 2] = m[ 8]; 
	r[ 3] = m[12];
	r[ 4] = m[ 1]; 
	r[ 6] = m[ 9]; 
	r[ 7] = m[13];
	r[ 8] = m[ 2]; 
	r[ 9] = m[ 6]; 
	r[11] = m[14];
	r[12] = m[ 3]; 
	r[13] = m[ 7]; 
	r[14] = m[11];
	r[15] = m[15]; 
	
	return r;
}

static Mat4
inverse_affine(Mat4 m)
{
	f32 t0 = m[10] * m[5] - m[ 6] * m[9];
	f32 t1 = m[ 2] * m[9] - m[10] * m[1];
	f32 t2 = m[ 6] * m[1] - m[ 2] * m[5];

	f32 idet = 1.0f / (m[0] * t0 + m[4] * t1 + m[8] * t2);

	t0 *= idet;
	t1 *= idet;
	t2 *= idet;

	f32 v0 = m[0] * idet;
	f32 v4 = m[4] * idet;
	f32 v8 = m[8] * idet;

	Mat4 r;
	r[ 0] = t0; 
	r[ 1] = t1; 
	r[ 2] = t2;
	r[ 3] = 0;
	r[ 4] = v8 * m[ 6] - v4 * m[10];
	r[ 5] = v0 * m[10] - v8 * m[ 2];
	r[ 6] = v4 * m[ 2] - v0 * m[ 6];
	r[ 7] = 0;
	r[ 8] = v4 * m[9] - v8 * m[5];
	r[ 9] = v8 * m[1] - v0 * m[9];
	r[10] = v0 * m[5] - v4 * m[1];
	r[11] = 0;
	r[12] = -(r[0] * m[12] + r[4] * m[13] + r[ 8] * m[14]);
	r[13] = -(r[1] * m[12] + r[5] * m[13] + r[ 9] * m[14]);
	r[14] = -(r[2] * m[12] + r[6] * m[13] + r[10] * m[14]);		
	r[15] = 1;

	return r;
}

static void
translate(Mat4& m, Vec3 p)
{
	m[12] = m[0] * p.x + m[4] * p.y + m[ 8] * p.z + m[12];
    m[13] = m[1] * p.x + m[5] * p.y + m[ 9] * p.z + m[13];
    m[14] = m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14];
    m[15] = m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15];
}

static void
set_position(Mat4& m, Vec3 p)
{
	m[12] = p.x; 
	m[13] = p.y; 
	m[14] = p.z;
}

static Vec3
get_position(Mat4 m)
{
	return { m[12], m[13], m[14] };
}

static void 
set_scale(Mat4& m, Vec3 s)
{
	m[ 0] = s.x; 
	m[ 5] = s.y; 
	m[10] = s.z;
}

static void 
scale(Mat4& m, Vec3 s)
{
	m[ 0] *= s.x; 
	m[ 1] *= s.x; 
	m[ 2] *= s.x;
	m[ 3] *= s.x;
	m[ 4] *= s.y;
	m[ 5] *= s.y;
	m[ 6] *= s.y;
	m[ 7] *= s.y;
	m[ 8] *= s.z;
	m[ 9] *= s.z;
	m[10] *= s.z;
	m[11] *= s.z;
}

static Vec3
get_scale(Mat4 m)
{
	return { m[0], m[5], m[10] };
}

static void 
rotate_x(Mat4& m, f32 rad) 
{
	Mat4 t = m;

    f32 s = sinf(rad);
    f32 c = cosf(rad);

    m[ 4] = t[ 4] * c + t[ 8] * s;
    m[ 5] = t[ 5] * c + t[ 9] * s;
    m[ 6] = t[ 6] * c + t[10] * s;
    m[ 7] = t[ 7] * c + t[11] * s;
    m[ 8] = t[ 8] * c - t[ 4] * s;
    m[ 9] = t[ 9] * c - t[ 5] * s;
    m[10] = t[10] * c - t[ 6] * s;
    m[11] = t[11] * c - t[ 7] * s;
}

static void
rotate_y(Mat4& m, f32 rad) 
{
	Mat4 t = m;

    f32 s = sinf(rad);
    f32 c = cosf(rad);

    m[ 0] = t[0] * c - t[ 8] * s;
    m[ 1] = t[1] * c - t[ 9] * s;
    m[ 2] = t[2] * c - t[10] * s;
    m[ 3] = t[3] * c - t[11] * s;
    m[ 8] = t[0] * s + t[ 8] * c;
    m[ 9] = t[1] * s + t[ 9] * c;
    m[10] = t[2] * s + t[10] * c;
    m[11] = t[3] * s + t[11] * c;
}

static void 
rotate_z(Mat4& m, f32 rad) 
{
	Mat4 t = m;

    f32 s = sinf(rad);
    f32 c = cosf(rad);
    
    m[0] = t[0] * c + t[4] * s;
    m[1] = t[1] * c + t[5] * s;
    m[2] = t[2] * c + t[6] * s;
    m[3] = t[3] * c + t[7] * s;
    m[4] = t[4] * c - t[0] * s;
    m[5] = t[5] * c - t[1] * s;
    m[6] = t[6] * c - t[2] * s;
    m[7] = t[7] * c - t[3] * s;
}

static void
set_rotation(Mat4& m, Vec4 q)
{
	f32 x2 = 2.0f * q.x; 
	f32 y2 = 2.0f * q.y; 
	f32 z2 = 2.0f * q.z;
	f32 xx = q.x * x2; 
	f32 xy = q.x * y2; 
	f32 xz = q.x * z2;
	f32 yy = q.y * y2;
	f32 yz = q.y * z2;
	f32 zz = q.z * z2;
	f32 wx = q.w * x2; 
	f32 wy = q.w * y2;
	f32 wz = q.w * z2;

	m[ 0] = 1.0f - (yy + zz);
	m[ 1] = xy + wz;
	m[ 2] = xz - wy;
	m[ 3] = 0.0f;
	m[ 4] = xy - wz;
	m[ 5] = 1.0f - (xx + zz);
	m[ 6] = yz + wx;
	m[ 7] = 0.0f;
	m[ 8] = xz + wy;
	m[ 9] = yz - wx;
	m[10] = 1.0f - (xx + yy);
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}

static Vec4
get_rotation(Mat4 m)
{
	Vec4 q;
	f32 t;

	if(m[10] < 0.0f)
	{
		if(m[0] > m[5])
		{
			t = 1.0f + m[0] - m[5] - m[10];
			q = { t, m[1] + m[4], m[8] + m[2], m[6] - m[9] };
		}
		else
		{
			t = 1.0f - m[0] + m[5] - m[10];
			q = { m[1] + m[4], t, m[6] + m[9], m[8] - m[2] };
		}
	}
	else
	{
		if(m[0] < -m[5])
		{
			t = 1.0f - m[0] - m[5] + m[10];
			q = { m[8] + m[2], m[6] + m[9], t, m[1] - m[4] };
		}
		else
		{
			t = 1.0f + m[0] + m[5] + m[10];
			q = { m[6] - m[9], m[8] - m[2], m[1] - m[4], t };
		}
	}

	return q * 0.5f / sqrtf(t);
}

static Mat4
compose(Vec3 p, Vec3 s, Vec4 r)
{
	Mat4 result = mat4_identity(); 

	set_rotation(result, r);
	scale(result, s);
	set_position(result, p);

	return result;
}

static Vec3
mul_point(Vec3 p, Mat4 m)
{
	return
	{
		m[0] * p.x + m[4] * p.y + m[ 8] * p.z + m[12],
		m[1] * p.x + m[5] * p.y + m[ 9] * p.z + m[13],
		m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14]
	};
}

static Vec3
mul_dir(Vec3 p, Mat4 m)
{
	return
	{
		m[0] * p.x + m[4] * p.y + m[ 8] * p.z,
		m[1] * p.x + m[5] * p.y + m[ 9] * p.z,
		m[2] * p.x + m[6] * p.y + m[10] * p.z
	};
}

static Vec3
mul_projection(Vec3 p, Mat4 m)
{
	f32 d = 1.0f / (m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15]);
	return mul_point(p,m) * d;
}