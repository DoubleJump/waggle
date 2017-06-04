typedef f32 (*ease_func)(f32 t);

struct Keyframe
{
	Vec2 left;
	Vec2 center;
	Vec2 right;
};

struct Animation_Property
{
	u32 hash;
	Float_Primitive type;
	u32 offset;
};

struct Animation_Curve
{
	Keyframe* frames;
	u32 num_frames;
	f32 value;
};

struct Animation
{
	Animation_Property* properties;
	u32 num_properties;

	Animation_Curve* curves;
	u32 num_curves;
	
	i32 loops;
	i32 num_loops;
	b32 is_playing;
	f32 start;
	f32 duration;
	f32 t;
	f32 scale;
};