static void bind_render_target(Render_Target* t);

static Render_Target*
new_render_target(u32 width, u32 height)
{
	auto rt = alloc(Render_Target);
	rt->width = width;
	rt->height = height;
	rt->color = rgba_texture(width, height, &default_sampler);
	rt->depth = depth_texture(width, height);
	bind_render_target(rt);
	return rt;
}

static Render_Target*
new_render_target(Vec4 view)
{
	return new_render_target(view.z, view.w);
}