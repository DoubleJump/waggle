static Vec3
eval(Bezier_Segment segment, f32 t)
{
	Vec3 result;
	for(auto i = 0; i < 3; ++i)
	{
		auto u = 1 - t;
		auto tt = t * t;
		auto uu = u * u;
		auto uuu = uu * u;
		auto ttt = tt * t;
		result[i] = (uuu * segment.a[i]) + 
					(3 * uu * t * segment.b[i]) + 
					(3 * u * tt * segment.c[i]) + 
					(ttt * segment.d[i]);
	}
	return result;
}

static void
eval(Animation_Curve* curve, f32 time)
{
	Bezier_Segment segment;
	u32 num_frames = curve->num_frames;

	//TODO: clamp, extrapolate etc
	

	auto frame = curve->frames; 
	for(auto i = 0; i < num_frames-1; ++i)
	{
		auto next = frame + 1;
		if(time >= frame->center.x && time <= next->center.x)
		{
			Vec2 delta = next->center - frame->center;
			auto t = clamp((time - frame->center.x) / delta.x, 0.0,1.0);
			segment.a = to_vec3(frame->center);
			segment.b = to_vec3(frame->right);
			segment.c = to_vec3(next->left);
			segment.d = to_vec3(next->center);

			curve->value = eval(segment, t).y;

			break;
		}
		frame++;
	}
}

static void
set_time(Animation* anim, f32 t)
{
	anim->t = t;
	auto num_curves = anim->num_curves;
	auto curve = anim->curves;
	for(auto i = 0; i < num_curves; ++i)
	{
		eval(curve, t);
		curve++;
	}
}

static void
update(Animation* anim, f32 dt)
{
	auto new_t = anim->t += dt;
	set_time(anim, new_t);
}


static void 
get_value(Animation* anim, const char* name, void* value)
{
	auto hash = djb_hash(name);
	auto num_properties = anim->num_properties;
	auto prop = anim->properties;
	for(auto i = 0; i < num_properties; ++i)
	{
		if(prop->hash == hash)
		{
			auto start = prop->offset;
			auto type = prop->type;
			auto end = start + get_stride(type);

			f32* val = (f32*)value;

			auto index = 0;
			auto curve = &anim->curves[start];
			for(auto j = start; j < end; ++j)
			{
				val[index] = curve->value;
				index++;
				curve++;
			}
			if(type == Float_Primitive::QUATERNION)
			{
				f32 tmp = val[0];
				val[0] = val[3];
				val[3] = tmp;
			}

			break;
		}
		prop++;
	}
}
	

static void
log(Animation* anim)
{
	printf("Num curves: %u\n", anim->num_curves);
	
	auto curve = anim->curves;
	for(auto i = 0; i < anim->num_curves; ++i)
	{
		auto num_frames = curve->num_frames;
		auto frame = curve->frames;
		for(auto j = 0; j < num_frames; ++j)
		{
			printf("[%f, %f]\n", frame->center.x, frame->center.y);
			frame++;
		}

		curve++;
	}
}