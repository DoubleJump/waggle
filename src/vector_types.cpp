struct Vec2
{
	f32 x, y;
	f32& operator[](int i){ return (&x)[i]; } 
};

struct Vec3
{
	f32 x, y, z;
	f32& operator[](int i){ return (&x)[i]; } 
};

static Vec3 vec3_up = {0,1,0};
static Vec3 vec3_down = {0,-1,0};
static Vec3 vec3_left = {-1,0,0};
static Vec3 vec3_right = {1,0,0};
static Vec3 vec3_forward = {0,0,1};
static Vec3 vec3_back = {0,0,-1};
static Vec3 vec3_zero = {0,0,0};
static Vec3 vec3_one = {1,1,1};

struct Vec4
{
	f32 x, y, z, w;
	f32& operator[](int i){ return (&x)[i]; } 
};

struct Mat3
{
	f32 m[9];
	f32& operator[](int i){ return m[i]; } 
};

struct Mat4
{
	f32 m[16];
	f32& operator[](int i){ return m[i]; }; 
};

struct Ray
{
	Vec3 origin;
	Vec3 direction;
	f32 length;
};

struct AABB
{
	Vec3 min;
	Vec3 max;
};

struct Bezier_Segment
{
	Vec3 a,b,c,d;
};

struct Triangle
{
	Vec3 a,b,c;
};

struct Plane
{
	Vec3 position;
	Vec3 normal;
};

struct Sphere
{
	Vec3 position;
	f32 radius;
};

struct Capsule
{
	Vec3 position;
	f32 radius;
	f32 height;
};

struct Hit_Info
{
	b32 hit;
	Vec3 point;
	Vec3 normal;
	f32 t;	
};

enum struct Float_Primitive
{
    F32 = 0,
    VEC2 = 1,
    VEC3 = 2,
    VEC4 = 3,
    QUATERNION = 4,
};

static u32
get_stride(Float_Primitive p)
{
	switch(p)
	{
		case Float_Primitive::F32: return 1;
		case Float_Primitive::VEC2: return 2;
		case Float_Primitive::VEC3: return 3;
		case Float_Primitive::VEC4: return 4;
		case Float_Primitive::QUATERNION: return 4;
	}
}