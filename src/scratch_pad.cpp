// INSTANCED NOISE WAVE
/*
app->quad = quad_mesh(0.02, 0.02, 0,0);

Vertex_Attribute attributes[] = 
{
    {"instance_position", 3, false},
};

auto v_count = 4096;
auto vertices = alloc_array(Vec3, v_count);
auto cell_size = 0.06f;
auto cols = 128;

for(auto i = 0; i < v_count; ++i)
{
	auto x = i % cols;
	auto z = i / cols;
	vertices[i] = {x * cell_size, 0, z * cell_size};
}

auto vb = &app->instance_buffer;
vb->instanced = true;
init_vertex_buffer(vb, attributes, ARRAY_COUNT(attributes),
					   (f32*)vertices, v_count*3);
bind_buffer(vb);
update_vertex_buffer(vb);
*/

// ALPHA BLEND MODES

static u32
convert_blend_function(u32 val)
{
	if(val == 0) return GL_ZERO;
	if(val == 1) return GL_ONE;
	if(val == 2) return GL_SRC_COLOR;
	if(val == 3) return GL_ONE_MINUS_SRC_COLOR;
	if(val == 4) return GL_DST_COLOR;
	if(val == 5) return GL_ONE_MINUS_DST_COLOR;
	if(val == 6) return GL_SRC_ALPHA;
	if(val == 7) return GL_ONE_MINUS_SRC_ALPHA;
	if(val == 8) return GL_DST_ALPHA;
	return GL_ONE_MINUS_DST_ALPHA;
}

static u32
convert_blend_op(u32 val)
{
	if(val == 0) return GL_FUNC_ADD;
	if(val == 1) return GL_FUNC_SUBTRACT;
	if(val == 2) return GL_FUNC_REVERSE_SUBTRACT;
	if(val == 3) return GL_MIN;
	return GL_MAX;
}

// ANIMATION
auto anim = animations["test_anim"];
update(anim, dt);
if(anim->t > 2) anim->t = 0;
get_value(anim, "position", &app->entity->position);
get_value(anim, "rotation", &app->entity->rotation);

// IM ANIMATION

auto ctx = app->gl_draw;
ctx->color = {1,1,0,1};

auto te = engine->time.elapsed;
auto tl = fmod(te,2.0);

for(auto i = 0; i < 4; ++i)
{
	auto start = i * 0.2;
	auto end = start + 0.4;
	auto lt = ease_in_out_cubic(range_step(start, end,tl));
	if(!lt) continue;

	ctx->color = lerp(col_navy, col_sunflower,lt);		

	f32 x = lerp(0,1,lt);
	f32 y = i * 0.2f;
	Vec3 left = {0,y,0};
	Vec3 right = {x, y+0.2f, 0}; 
	push_quad(ctx, left, right);
}

for(auto i = 0; i < 4; ++i)
{
	auto start = 0.4 + (i * 0.2);
	auto end = start + 0.4;

	auto lt = ease_in_out_cubic(range_step(start,end,tl));
	if(!lt) continue;

	ctx->color = lerp(col_sunflower,col_navy,lt);		

	f32 x = lerp(1,0,lt);
	f32 y = i * 0.2f;
	Vec3 left = {0,y,0};
	Vec3 right = {x, y+0.2f, 0}; 
	push_quad(ctx, left, right);
}

// SURFACE NOISE WAVE

set_blend_mode(Blend_Mode::ADD);
{
    auto shader = shaders["grid_noise"];
	use_shader(shader);
    set_uniform("mvp", camera->view_projection);
    set_uniform("time", (f32)engine->time.elapsed);
	draw_mesh(meshes["grid"]);
}

// EQUI RECTANGULAR MAPPING

{
	auto shader = shaders["reflection"];
	use_shader(shader);
	
	set_uniform("view", camera->view);
	set_uniform("projection", camera->projection);
	set_uniform("normal_matrix", camera->normal_matrix);
	set_uniform("env_map", textures["rect_map"]);
	set_uniform("inv_view_matrix", camera->world_matrix);
	set_uniform("bias", app->bias);
	set_uniform("scale", app->scale);
	set_uniform("power", app->power);
	draw_mesh(meshes["sphere"]);
}

// SPHERE VS MESH COLLISION RESPONSE

auto fl = app->free_look;
Hit_Info info;
auto mesh = meshes["walls"];
auto radius = 0.2;
if(sphere_vs_mesh(&info, Sphere{fl->camera->position, radius}, mesh))
{
	auto proj = project(fl->velocity, info.normal);
	fl->velocity = fl->velocity - proj;
	fl->camera->position = info.point + (info.normal * radius);
}

// OLD CYLINDER COLLISION
/*
static Vec3
cylinder_sweep_triangle(GL_Draw* ctx, Hit_Info* info, Cylinder c, Vec3 velocity, Triangle tri)
{
	info->hit = false;
	Vec3 response = {0,0,0};

	Plane plane = plane_from_triangle(tri);
	Vec3 start = c.position;
	Vec3 end = start + velocity;
	Vec3 dir = normalize(velocity);
	f32 mag = length(velocity);

	// make sure we test as least as far as the cylinder radius
	if(mag < c.radius) mag = c.radius;

	auto R = ortho_normalize(dir, {0,1,0});
	auto U = ortho_normalize(R, dir);
	R *= c.radius;
	U *= c.height;

	// create polygon cross section facing direction of velocity
	Vec3 tl = start - R + U;
	Vec3 tr = start + R + U;
	Vec3 bl = start - R - U;
	Vec3 br = start + R - U;

	b32 collision = true;

	collision = false;

	// project polygon onto triangle plane in direction of velocity
	Ray ray = {tl, dir, F32_MAX};
	if(ray_plane(info, ray, plane) && info->t < mag) 
	{
		collision = true;
		if(info->t < 0) collision = false;
	}

	Vec3 pA = info->point;

	ray.origin = tr;
	if(ray_plane(info, ray, plane) && info->t < mag)
	{
		collision = true;
		if(info->t < 0) collision = false;
	}
	Vec3 pB = info->point;

	ray.origin = br;
	if(ray_plane(info, ray, plane) && info->t < mag)
	{
		collision = true;
		if(info->t < 0) collision = false;
	}
	Vec3 pC = info->point;

	ray.origin = bl;
	if(ray_plane(info, ray, plane) && info->t < mag)
	{
		collision = true;
		if(info->t < 0) collision = false;
	}
	Vec3 pD = info->point;

	ctx->color = col_sunflower;	
	draw_line(ctx, pA, pB);
	draw_line(ctx, pB, pC);
	draw_line(ctx, pC, pD);
	draw_line(ctx, pD, pA);

	if(!collision)
	{
		info->hit = false;
		return response;
	}

	collision = false;

	//FIXME: wont work if triangle in enclosed entirely by the projection

	// test if the projected polygon intersects the triangle
	if(line_line(info, pA, pB, tri.a, tri.b)) collision = true;
	else if(line_line(info, pA, pB, tri.b, tri.c)) collision = true;
	else if(line_line(info, pA, pB, tri.c, tri.a)) collision = true;

	else if (line_line(info, pB, pC, tri.a, tri.b)) collision = true;
	else if(line_line(info, pB, pC, tri.b, tri.c)) collision = true;
	else if(line_line(info, pB, pC, tri.c, tri.a)) collision = true;

	else if (line_line(info, pC, pD, tri.a, tri.b)) collision = true;
	else if(line_line(info, pC, pD, tri.b, tri.c)) collision = true;
	else if(line_line(info, pC, pD, tri.c, tri.a)) collision = true;

	else if (line_line(info, pD, pA, tri.a, tri.b)) collision = true;
	else if(line_line(info, pD, pA, tri.b, tri.c)) collision = true;
	else if(line_line(info, pD, pA, tri.c, tri.a)) collision = true;


	else if(point_in_triangle(pA, tri)) collision = true;
	else if(point_in_triangle(pB, tri)) collision = true;
	else if(point_in_triangle(pC, tri)) collision = true;
	else if(point_in_triangle(pD, tri)) collision = true;

	if(!collision) 
	{
		info->hit = false;
		return response;
	}

	Vec3 center = (pA + pB + pC + pD) / 4.0;

	Ray response_ray;
	response_ray.origin = center;
	response_ray.direction = -dir;

	Plane offset;
	offset.position = center + (info->normal * c.radius);
	offset.normal = -info->normal;

	f32 d = dot(offset.normal, response_ray.direction);
	f32 t = dot((offset.position - response_ray.origin), offset.normal) / d;

	response = center -(dir * t);

	info->hit = true;
	info->t = t;
	info->normal = plane.normal;
	info->point = response_ray.origin;
	return response;
}

static b32
sphere_vs_mesh(Hit_Info* info, Sphere s, Mesh* mesh, Vec3 last)
{
	auto vb = &mesh->vertex_buffer;
	auto ib = &mesh->index_buffer;
	
	auto stride = vb->stride;
	auto vertex_data = vb->data;

	//TODO: optional matrix param
	//NOTE: assumes index buffer
	
	auto index_data = ib->data;
	auto n = ib->count;
	Hit_Info closest;
	closest.t = F32_MAX;
	closest.hit = false;

	f32 min_dist = F32_MAX;
	for(auto i = 0; i < n; i+=3)
	{
		auto ia = index_data[i  ] * stride;
		auto ib = index_data[i+1] * stride;
		auto ic = index_data[i+2] * stride;
		
		Triangle t;
		t.a = *(Vec3*)(vertex_data + ia);
		t.b = *(Vec3*)(vertex_data + ib);
		t.c = *(Vec3*)(vertex_data + ic);

		if(sphere_vs_triangle(info, s, t))
		{
			auto dist = sqr_distance(last, info->point);

			//if(info->t < closest.t)
			if(dist < min_dist)
			{
				min_dist = dist;
				closest = *info;
				closest.hit = true;
			}
		}
	}
	
	*info = closest;
	return info->hit;
}
*/