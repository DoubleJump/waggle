static Mat3 
mat3_identity()
{ 
	return
	{
		1,0,0,
		0,1,0,
		0,0,1
	};
};

static Mat3
operator * (Mat3 a, Mat3 b)
{
	Mat3 r;

	r[0] = a[0] * b[0] + a[1] * b[3] + a[2] * b[6];
	r[1] = a[0] * b[1] + a[1] * b[4] + a[2] * b[7];
	r[2] = a[0] * b[2] + a[1] * b[5] + a[2] * b[8];
	r[3] = a[3] * b[0] + a[4] * b[3] + a[5] * b[6];
	r[4] = a[3] * b[1] + a[4] * b[4] + a[5] * b[7];
	r[5] = a[3] * b[2] + a[4] * b[5] + a[5] * b[8];
	r[6] = a[6] * b[0] + a[7] * b[3] + a[8] * b[6];
	r[7] = a[6] * b[1] + a[7] * b[4] + a[8] * b[7];
	r[8] = a[6] * b[2] + a[7] * b[5] + a[8] * b[8];

	return r;
}

static Mat3
operator *= (Mat3& a, Mat3 b)
{
	a = a * b;
	return a;
}

static Mat3
operator - (Mat3 m)
{
	Mat3 r;

    r[0] = m[4] * m[8] - m[5] * m[7];
    r[1] = m[2] * m[7] - m[1] * m[8];
    r[2] = m[1] * m[5] - m[2] * m[4];
    r[3] = m[5] * m[6] - m[3] * m[8];
    r[4] = m[0] * m[8] - m[2] * m[6];
    r[5] = m[2] * m[3] - m[0] * m[5];
    r[6] = m[3] * m[7] - m[4] * m[6];
    r[7] = m[1] * m[6] - m[0] * m[7];
    r[8] = m[0] * m[4] - m[1] * m[3];

    f32 det = m[0] * r[0] + m[1] * r[3] + m[2] * r[6];
    if(fabs(det) <= EPSILON)
    {
        return mat3_identity();
    }

    f32 inv_det = 1.0f / det;
    r[0] *= inv_det;
    r[1] *= inv_det;
    r[2] *= inv_det;
    r[3] *= inv_det;
    r[4] *= inv_det;
    r[5] *= inv_det;
    r[6] *= inv_det;
    r[7] *= inv_det;
    r[8] *= inv_det;

    return r;
}

static Mat3
to_mat3(Mat4 m)
{
	Mat3 r;
	r[0] = m[0]; 
	r[1] = m[1]; 
	r[2] = m[2];
	r[3] = m[4]; 
	r[4] = m[5]; 
	r[5] = m[6];
	r[6] = m[8]; 
	r[7] = m[9]; 
	r[8] = m[10];
	return r;
}

static f32
determinant(Mat3 m)
{
	return m[0] * (m[4] * m[8] - m[5] * m[7]) -
      	   m[1] * (m[3] * m[8] - m[5] * m[6]) +
      	   m[2] * (m[3] * m[7] - m[4] * m[6]);
}

static Mat3
transpose(Mat3 m)
{
	Mat3 r;
	r[0] = m[0];
	r[1] = m[3];
	r[2] = m[6]; 
	r[3] = m[1];
	r[4] = m[4];
	r[5] = m[7]; 
	r[6] = m[2]; 
	r[7] = m[5];
	r[8] = m[8];
	return r;
}

static void
set_position(Mat3& m, Vec3 p)
{
	m[2] = p.x;
	m[5] = p.y;
}

static Vec3
get_position(Mat3 m)
{
	return { m[2], m[5], 0.0f };
}

static void
set_rotation(Mat3& m, Vec4 q)
{
	f32 x2 = 2.0f * m[0]; 
	f32 y2 = 2.0f * m[1]; 
	f32 z2 = 2.0f * m[2];
	f32 xx = m[0] * x2; 
	f32 xy = m[0] * y2; 
	f32 xz = m[0] * z2;
	f32 yy = m[1] * y2;
	f32 yz = m[1] * z2;
	f32 zz = m[2] * z2;
	f32 wx = m[3] * x2; 
	f32 wy = m[3] * y2;
	f32 wz = m[3] * z2;

	m[0] = 1.0f - (yy + zz);
	m[1] = xy + wz;
	m[2] = xz - wy;
	m[3] = xy - wz;
	m[4] = 1.0f - (xx + zz);
	m[5] = yz + wx;
	m[6] = xz + wy;
	m[7] = yz - wx;
	m[8] = 1.0f - (xx + yy);
}

static void
set_rotation(Mat3& m, f32 r)
{
	f32 theta = radians(r);
	f32 st = sinf(theta);
	f32 ct = cosf(theta);

	m[0] = ct;
	m[1] = st;
	m[3] = -st;
	m[4] = ct;
}

static void
scale(Mat3& m, Vec3 s)
{
	m[0] *= s.x;
	m[4] *= s.y;
}

static Mat3
compose(Vec3 p, Vec3 s, f32 rot)
{
	Mat3 r = mat3_identity();
	set_rotation(r, rot);
	scale(r, s);
	set_position(r, p);
	return r;
}

static Vec3
mul_point(Mat3 m, Vec3 p)
{
	return { m[0] * p.x + m[1] * p.y + m[2], m[3] * p.x + m[4] * p.y + m[5], 0.0f };
}