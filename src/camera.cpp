static void 
update_camera_projection(Camera* c)
{
	if(c->orthographic)
	{
		c->projection = ortho_projection(c->size * c->aspect,c->size,c->near,c->far);
	}
	else 
	{
		c->projection = perspective_projection(c->near, c->far, c->aspect, c->fov);
	}

	//TODO: world to screen matrices
}

static Camera*
new_camera(f32 near, f32 far, f32 fov, Vec4 view)
{
	auto c = alloc(Camera);
	init_entity((Entity*)c);
	c->entity_type = Entity_Type::CAMERA;
	c->near = near;
	c->far = far;
	c->fov = fov;
	c->size = 1;
	c->aspect = view.z / view.w;	
	return c;
}

static Camera*
perspective_camera(f32 near, f32 far, f32 fov, Vec4 view)
{
	auto c = new_camera(near, far, fov, view);
	c->orthographic = false;
	update_camera_projection(c);
	return c;
}

static Camera*
ortho_camera(f32 near, f32 far, f32 size, Vec4 view)
{
	auto c = new_camera(near, far, 60, view);
	c->orthographic = true;
	c->size = size;
	update_camera_projection(c);
	return c;
}

static Camera* 
ui_camera(Vec4 view)
{
	auto c = new_camera(0.01, 10, 60, view);
	c->orthographic = true;
	c->size = view.w;

	auto hw = view.z / 2;
	auto hh = view.w / 2;
	c->position = {view.x + hw, view.y - hh, 1.0f};
	update_camera_projection(c);
    return c;
}

static void 
update(Camera* c)
{
	update_camera_projection(c);
	c->view = inverse_affine(c->world_matrix);
	c->view_projection = c->view * c->projection;
	c->normal_matrix = transpose(-to_mat3(c->view));
}

static Ray
create_camera_ray(Camera* camera, f32 length)
{
	Vec3 fwd = mul_dir(vec3_forward, camera->world_matrix);
	Vec3 origin = mul_point(camera->position, camera->world_matrix);

	Ray ray = {origin, fwd, length};
	return ray;
}