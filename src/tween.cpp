static f32
clamp_step(f32 start, f32 end, f32 t)
{
	return clamp((t - start) / (end - start), 0.0, 1.0);
}

static f32
range_step(f32 start, f32 end, f32 t)
{
	auto val = (t - start) / (end - start);
	if(val < 0 || val > 1) return 0;
	return val;
}


/*
static f32
tween(f32 from, f32 to, f32 start, f32 end, f32 t, ease_func ease)
{
	//anim ctx for time, ease etc??
	auto et = ease(linear_step(start, end, t));
	return lerp(from, to, et);
}

static Vec2
tween(Vec2 from, Vec2 to, f32 start, f32 end, f32 t, ease_func ease)
{
	auto et = ease(linear_step(start, end, t));
	return lerp(from, to, et);
}

static Vec3
tween(Vec3 from, Vec3 to, f32 start, f32 end, f32 t, ease_func ease)
{
	auto et = ease(linear_step(start, end, t));
	return lerp(from, to, et);
}

static Vec4
tween(Vec4 from, Vec4 to, f32 start, f32 end, f32 t, ease_func ease)
{
	auto et = ease(linear_step(start, end, t));
	return lerp(from, to, et);
}
*/