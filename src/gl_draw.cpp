static Mesh*
line_mesh_buffer(memsize vertex_count, Vertex_Attribute* attributes, i32 num_attributes)
{
	auto mesh = alloc(Mesh);
	mesh->use_index_buffer = false;
	mesh->primitive = Mesh_Primitive::LINES;
	
	auto vb = &mesh->vertex_buffer;
	set_vertex_attributes(vb, attributes, num_attributes);
	alloc_vertex_buffer(vb, vertex_count);
	vb->count = 0;

	bind_mesh(mesh);
	return mesh;
}

static Mesh*
triangle_mesh_buffer(memsize vertex_count, Vertex_Attribute* attributes, i32 num_attributes)
{
	auto mesh = alloc(Mesh);
	mesh->use_index_buffer = true;
	mesh->primitive = Mesh_Primitive::TRIANGLES;
	
	auto vb = &mesh->vertex_buffer;
	auto ib = &mesh->index_buffer;
	set_vertex_attributes(vb, attributes, num_attributes);
	alloc_vertex_buffer(vb, vertex_count);
	alloc_index_buffer(ib, vertex_count * 3);
	vb->count = 0;
	ib->count = 0;

	bind_mesh(mesh);
	return mesh;
}

static GL_Draw*
new_gl_draw(u32 buffer_size = 4096)
{
	auto r = alloc(GL_Draw);
	r->color = {1,1,1,1};

	Vertex_Attribute line_attributes[] = 
	{
	    Position_Attribute,
	    Color_Attribute
	};
	r->lines = line_mesh_buffer(buffer_size, line_attributes, ARRAY_COUNT(line_attributes));
	
	Vertex_Attribute tri_attributes[] = 
	{
	    Position_Attribute,
	    UV_Attribute,
	    Vertex_Attribute("radius", 1, false),
	    Color_Attribute
	};
	r->triangles = triangle_mesh_buffer(buffer_size, tri_attributes, ARRAY_COUNT(tri_attributes));

	auto assets = engine->assets;
	r->line_shader = assets.shaders["gl_lines"];
	r->triangle_shader = assets.shaders["rect"];
	
	auto font = assets.fonts["input_mono"];
	auto text_shader = assets.shaders["msdf_text"];
	auto style = new_text_style(font, text_shader, 0.001f, {0.8,0.8,0.8,1});
	r->text = new_text(style, 4096, {0,0,0,0});

	return r;
}

static void
reset_gl_draw(GL_Draw* ctx)
{
	ctx->matrix = mat4_identity();
	ctx->color = {1,1,1,1};
	clear_mesh_buffers(ctx->lines);
	clear_mesh_buffers(ctx->triangles);
	reset(ctx->text);
}

static void
render_gl_draw(GL_Draw* ctx, Camera* camera)
{
	if(ctx->lines->vertex_buffer.count > 0)
	{
		update_mesh(ctx->lines);
		use_shader(ctx->line_shader);
		set_uniform("mvp", camera->view_projection);
		draw_mesh(ctx->lines);
	}
	if(ctx->triangles->vertex_buffer.count > 0)
	{
		update_mesh(ctx->triangles);
		use_shader(ctx->triangle_shader);
		set_uniform("mvp", camera->view_projection);
		draw_mesh(ctx->triangles);
	}
	
	update_mesh(ctx->text->mesh);
	draw_text(ctx->text, camera);
	
	reset_gl_draw(ctx);
}

// LINES

static void
push_line_vertex(GL_Draw* ctx, Vec3 position)
{
	auto p = mul_point(position, ctx->matrix);
	auto vb = &ctx->lines->vertex_buffer;

	auto d = (GL_Line_Vertex*)&vb->data[vb->offset];
	d->position = p;
	d->color = ctx->color;
	vb->offset += vb->stride;
	vb->count++;
	vb->needs_update = true;
}

static void
draw_line(GL_Draw* ctx, Vec3 start, Vec3 end)
{
	push_line_vertex(ctx, start);
	push_line_vertex(ctx, end);
}

static void
draw_point(GL_Draw* ctx, Vec3 point, f32 size)
{
	Vec3 lft = {point.x - size,point.y, point.z};
	Vec3 rht = {point.x + size,point.y, point.z};

	Vec3 up  = {point.x, point.y + size, point.z};
	Vec3 dwn = {point.x, point.y - size, point.z};

	Vec3 fwd = {point.x, point.y, point.z + size};
	Vec3 bck = {point.x, point.y, point.z - size};

	draw_line(ctx,  up, dwn);
	draw_line(ctx, lft, rht);
	draw_line(ctx, fwd, bck);
}

static void
draw_normal(GL_Draw* ctx, Vec3 origin, Vec3 normal, f32 length)
{
	draw_line(ctx, origin, origin + (normal * length));
}

static void
draw_plane(GL_Draw* ctx, Plane plane, f32 size)
{
	auto v = ortho_normalize(plane.normal, {0,1,0});
	auto u = ortho_normalize(v, plane.normal);

	draw_line(ctx, plane.position, plane.position + u);
	draw_line(ctx, plane.position, plane.position + v);
	draw_normal(ctx, plane.position, plane.normal, 1.0);
}

static void
draw_ray(GL_Draw* ctx, Ray ray)
{
	draw_normal(ctx, ray.origin, ray.direction, ray.length);
}

static void
draw_hit(GL_Draw* ctx, Hit_Info info)
{
	if(info.hit) draw_normal(ctx, info.point, info.normal, 1.0);
}

static void
draw_wire_rectangle(GL_Draw* ctx, Vec4 r)
{
	Vec3 tl = {r.x,r.y,0};
	Vec3 tr = {r.x + r.z, r.y,0};
	Vec3 bl = {r.x, r.y - r.w,0};
	Vec3 br = {r.x + r.z, r.y - r.w, 0};

	draw_line(ctx, tl, tr);
	draw_line(ctx, tr, br);
	draw_line(ctx, br, bl);
	draw_line(ctx, bl, tl);
}

static void
draw_wire_rectangle(GL_Draw* ctx, Vec3 p, f32 width, f32 height)
{
	auto r = rectangle_from_width_height(p, width, height);
	draw_wire_rectangle(ctx, r);
}

static void
draw_wire_cube(GL_Draw* ctx, Vec3 position, f32 width, f32 height, f32 depth)
{
	Vec3 p = position;
	f32 hw = width / 2;
	f32 hh = height / 2;
	f32 hd = depth / 2;

	Vec3 tlf = {p.x-hw, p.y+hh, p.z+hd};
	Vec3 trf = {p.x+hw, p.y+hh, p.z+hd};
	Vec3 blf = {p.x-hw, p.y-hh, p.z+hd};
	Vec3 brf = {p.x+hw, p.y-hh, p.z+hd};
	Vec3 tlb = {p.x-hw, p.y+hh, p.z-hd};
	Vec3 trb = {p.x+hw, p.y+hh, p.z-hd};
	Vec3 blb = {p.x-hw, p.y-hh, p.z-hd};
	Vec3 brb = {p.x+hw, p.y-hh, p.z-hd};

	draw_line(ctx, tlf, trf);
	draw_line(ctx, trf, brf);
	draw_line(ctx, brf, blf);
	draw_line(ctx, blf, tlf);

	draw_line(ctx, tlb, trb);
	draw_line(ctx, trb, brb);
	draw_line(ctx, brb, blb);
	draw_line(ctx, blb, tlb);

	draw_line(ctx, tlf, tlb);
	draw_line(ctx, trf, trb);
	draw_line(ctx, brf, brb);
	draw_line(ctx, blf, blb);
}

static void
draw_wire_circle(GL_Draw* ctx, Vec3 position, f32 radius, i32 segments)
{
	auto theta = TAU / segments;
	auto tf = tanf(theta);
	auto cf = cosf(theta);

	Vec3 p = {radius,0,0};
	Vec3 l = p;

	for(auto i = 0; i < segments; ++i)
	{
		auto tx = -p.y;
		auto ty =  p.x;
		p.x += (tx * tf); 
		p.y += (ty * tf);

		p *= cf;

		draw_line(ctx,position+l, position+p);
		l = p;
	}
}

static void
draw_wire_arc(GL_Draw* ctx, Vec3 position, f32 radius, f32 start, f32 end, i32 segments)
{
	auto srad = start * DEG2RAD;
	auto erad = end * DEG2RAD;
	auto theta = (erad - srad) / f32(segments);
	auto tf = tanf(theta);
	auto cf = cosf(theta);
	auto cs = cosf(srad) * radius;
	auto ss = sinf(srad) * radius; 

	Vec3 p = {cs,ss,0};
	Vec3 l = p;

	for(auto i = 0; i < segments; ++i)
	{
		auto tx = -p.y;
		auto ty =  p.x;
		p.x += (tx * tf); 
		p.y += (ty * tf);

		p *= cf;

		draw_line(ctx,position+l, position+p);
		l = p;
	}
}

static void
draw_wire_sphere(GL_Draw* ctx, Vec3 position, f32 radius)
{
	ctx->matrix = mat4_identity();
	set_position(ctx->matrix, position);
	draw_wire_circle(ctx, {0,0,0}, radius, 32);
	rotate_x(ctx->matrix, 90 * DEG2RAD);
	draw_wire_circle(ctx, {0,0,0}, radius, 32);
	rotate_y(ctx->matrix, 90 * DEG2RAD);
	draw_wire_circle(ctx, {0,0,0}, radius, 32);
	ctx->matrix = mat4_identity();
}

static void
draw_wire_capsule(GL_Draw* ctx, Capsule c)
{
	Vec3 top = c.position;
	Vec3 btm = c.position;
	top.y += c.height / 2;
	btm.y -= c.height / 2;
	
	ctx->matrix = mat4_identity();
	set_position(ctx->matrix, top);
	rotate_x(ctx->matrix, 90 * DEG2RAD);
	draw_wire_circle(ctx, {0,0,0}, c.radius, 32);
	set_position(ctx->matrix, btm);
	draw_wire_circle(ctx, {0,0,0}, c.radius, 32);

	ctx->matrix = mat4_identity();
	set_position(ctx->matrix, top);
	draw_wire_arc(ctx, {0,0,0}, c.radius, 0,180,16);
	rotate_y(ctx->matrix, 90 * DEG2RAD);
	draw_wire_arc(ctx, {0,0,0}, c.radius, 0,180,16);

	ctx->matrix = mat4_identity();
	set_position(ctx->matrix, btm);
	draw_wire_arc(ctx, {0,0,0}, c.radius, 180,360,16);
	rotate_y(ctx->matrix, 90 * DEG2RAD);
	draw_wire_arc(ctx, {0,0,0}, c.radius, 180,360,16);

	ctx->matrix = mat4_identity();
	top.x += c.radius;
	btm.x += c.radius;
	draw_line(ctx, top, btm);
	top.x -= c.radius * 2;
	btm.x -= c.radius * 2;
	draw_line(ctx, top, btm);

	top.x = c.position.x;
	top.z = c.position.z + c.radius;
	btm.x = c.position.x;
	btm.z = c.position.z + c.radius;
	draw_line(ctx, top, btm);
	top.z -= c.radius * 2;
	btm.z -= c.radius * 2;
	draw_line(ctx, top, btm);
}

static void
draw_matrix(GL_Draw* ctx, Mat4 m)
{
	ctx->matrix = m;
	ctx->color = {1,0,0,1};
	draw_line(ctx, {0,0,0}, {1,0,0});

	ctx->color = {0,1,0,1};
	draw_line(ctx, {0,0,0}, {0,1,0});

	ctx->color = {0,0,1,1};
	draw_line(ctx, {0,0,0}, {0,0,1});

	ctx->matrix = mat4_identity();
}

static void
draw_bounds(GL_Draw* ctx, AABB bounds)
{
	auto w = bounds.max.x - bounds.min.x;
	auto h = bounds.max.y - bounds.min.y;
	auto d = bounds.max.z - bounds.min.z;

	draw_wire_cube(ctx, {0,0,0}, w,h,d);
}

static void
draw_wire_triangle(GL_Draw* ctx, Triangle t)
{
	draw_line(ctx, t.a, t.b);
	draw_line(ctx, t.b, t.c);
	draw_line(ctx, t.c, t.a);
}

static void
draw_wire_mesh(GL_Draw* ctx, Mesh* mesh, b32 draw_normals = false)
{
	// TODO: matrix
	auto vb = &mesh->vertex_buffer;
	
	auto stride = vb->stride;
	auto vertex_data = vb->data;
	
	if(mesh->use_index_buffer)
	{
		auto ib = &mesh->index_buffer;
		auto index_data = ib->data;
		auto n = ib->count;
		for(auto i = 0; i < n; i+=3)
		{
			auto ia = index_data[i  ] * stride;
			auto ib = index_data[i+1] * stride;
			auto ic = index_data[i+2] * stride;
			
			auto ta = *(Vec3*)(vertex_data + ia);
			auto tb = *(Vec3*)(vertex_data + ib);
			auto tc = *(Vec3*)(vertex_data + ic);

			Triangle tri = {ta,tb,tc};
			draw_wire_triangle(ctx, tri);

			if(draw_normals)
			{
				auto center = get_center(tri);
				auto normal = get_normal(tri);
				draw_line(ctx, center, center + normal);
			}
		}
	}
	else
	{
		auto n = vb->count;
		for(auto i = 0; i < n; i+=3)
		{
			auto ia = (i  ) * stride;
			auto ib = (i+1) * stride;
			auto ic = (i+2) * stride;

			auto ta = *(Vec3*)(vertex_data + ia);
			auto tb = *(Vec3*)(vertex_data + ib);
			auto tc = *(Vec3*)(vertex_data + ic);
			draw_wire_triangle(ctx, {ta,tb,tc});
		}
	}
}


static void
draw_wire_camera(GL_Draw* ctx, Camera* camera)
{
	ctx->matrix = camera->world_matrix;
	auto inv = -camera->projection;

	Vec3 tip = {0,0,1};
	Vec3 tl = mul_point({-1, 1,0}, inv);
	Vec3 tr = mul_point({ 1, 1,0}, inv); 
	Vec3 bl = mul_point({-1,-1,0}, inv); 
	Vec3 br = mul_point({ 1,-1,0}, inv); 

	draw_line(ctx, tip, tl);
	draw_line(ctx, tip, tr);
	draw_line(ctx, tip, bl);
	draw_line(ctx, tip, br);
	draw_line(ctx, tl, tr);
	draw_line(ctx, tr, br);
	draw_line(ctx, br, bl);
	draw_line(ctx, bl, tl);

	ctx->matrix = mat4_identity();
}

/*
static void 
draw_bezier(GL_Draw* ctx, Curve* c, i32 segments)
{
	f32 step = 1.0 / segments;
	auto t = step;
	for(auto i = 0; i < segments; ++i)
	{
		Vec3 point = eval_curve(curve, t);
		push_line_vertex(ctx, point);
		t += step;
	}
}
*/


// QUADS

static void
push_polygon(GL_Draw* ctx, GL_Quad_Vertex* verts, u32 vert_count, u32*indices, u32 index_count)
{
	auto vb = &ctx->triangles->vertex_buffer;
	auto v = ((GL_Quad_Vertex*)vb->data) + vb->offset;

	for(u32 i = 0; i < vert_count; ++i)
	{
		v[i] = verts[i];
	}
	vb->offset += vert_count;
	vb->count += vert_count;
	vb->needs_update = true;

	auto ib = &ctx->triangles->index_buffer;
	auto i = ib->offset;
	auto to = ib->triangle_offset;
	auto index = ib->data + i;

	auto max_index = 0;
	for(u32 i = 0; i < index_count; ++i)
	{
		auto src = indices[i];
		index[i] = to + src;
		if(src > max_index) max_index = src;
	}

	ib->offset += index_count;
	ib->triangle_offset += (max_index+1);
	ib->count = ib->offset;
	ib->needs_update = true;
}


static void
push_quad(GL_Draw* ctx, Vec3 a, Vec3 b, f32 radius = 0)
{
	auto vb = &ctx->triangles->vertex_buffer;
	auto v = ((GL_Quad_Vertex*)vb->data) + vb->offset;

	Vec3 tl = {minimum(a.x, b.x), maximum(a.y, b.y), minimum(a.z,b.z)};
	Vec3 br = {maximum(a.x, b.x), minimum(a.y, b.y), maximum(a.z,b.z)};
	Vec3 tr = {br.x, tl.y, tl.z};
	Vec3 bl = {tl.x, br.y, br.z};

	v[0] = {bl, {0,0}, radius, ctx->color};
	v[1] = {br, {1,0}, radius, ctx->color};
	v[2] = {tl, {0,1}, radius, ctx->color};
	v[3] = {tr, {1,1}, radius, ctx->color};

	vb->offset += 4;
	vb->count += 4;
	vb->needs_update = true;

	auto ib = &ctx->triangles->index_buffer;
	auto i = ib->offset;
	auto to = ib->triangle_offset;
	auto index = ib->data + i;
	index[0] = to + 0;
	index[1] = to + 1;
	index[2] = to + 3;
	index[3] = to + 0;
	index[4] = to + 3;
	index[5] = to + 2;

	ib->offset += 6;
	ib->triangle_offset += 4;
	ib->count = ib->offset;
	ib->needs_update = true;
}

static void
draw_quad(GL_Draw* ctx, Vec4 r, f32 radius, f32 depth)
{
	push_quad(ctx, {r.x,r.y,depth}, {r.x + r.z, r.y + r.w,depth}, radius);
}

static void
draw_quad(GL_Draw* ctx, Vec4 r, f32 radius = 0)
{
	draw_quad(ctx, r, radius, 0);
}

static void
draw_text(GL_Draw* ctx, const char* text, Vec3 position)
{
	ctx->text->last_line_index = ctx->text->index;
	ctx->text->pen = position;
	ctx->text->style->color = ctx->color;
	add_text(ctx->text, text);
}

static void
draw_value(GL_Draw* ctx, f32 value, Vec3 position)
{
	char str[36];
	to_string(str, value);
	draw_text(ctx, str, position);
}

static void
draw_value(GL_Draw* ctx, Vec2 value, Vec3 position)
{
	char str[36];
	to_string(str, value);
	draw_text(ctx, str, position);
}

static void
draw_value(GL_Draw* ctx, Vec3 value, Vec3 position)
{
	char str[36];
	to_string(str, value);
	draw_text(ctx, str, position);
}

static void
draw_value(GL_Draw* ctx, Vec4 value, Vec3 position)
{
	char str[36];
	to_string(str, value);
	draw_text(ctx, str, position);
}