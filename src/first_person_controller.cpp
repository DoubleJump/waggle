struct First_Person_Controller : Entity
{
	Entity* head;
	Camera* camera;
	f32 height;
	f32 radius;
	Vec3 acceleration;
	Vec3 velocity;
	Vec3 look_angle;
	f32 move_speed;
	f32 move_friction;
	f32 rotate_speed;
	f32 vertical_look_limit;
	f32 max_walk_angle;
};

static First_Person_Controller*
new_first_person_controller(Scene* scene)
{
	auto fp = alloc(First_Person_Controller);
	init_entity(fp);

	fp->head = new_entity(NULL);
	fp->head->position.y = 0.25;

	auto camera = perspective_camera(-0.42, 100, 60, engine->view);
	camera->position.z = -1.0;
	set_parent(camera, fp->head);
	set_parent(fp->head, fp);
	fp->camera = camera;

	if(scene)
	{
		add_to_scene(fp, scene);
		add_to_scene(fp->head, scene);
		add_to_scene(fp->camera, scene);
	}

	fp->height = 0.5;
	fp->radius = 0.25;
	fp->position = {0,0.5,0};
	fp->velocity = {0,0,0};
	fp->look_angle = {0,0,0};
	fp->move_speed = 0.4;
	fp->move_friction = 0.9;
	fp->rotate_speed = 1.3;
	fp->vertical_look_limit = 80;
	fp->active = true;
	return fp;
}

static void
update(First_Person_Controller* fp, f32 dt)
{
	update(fp->camera);
	
	//if(key_down(Key_Code::F)) fp->active = !fp->active;
	//if(!fp->active)  return;

	auto rotate_speed = fp->rotate_speed * dt;
	fp->look_angle.x -= _input_ctx->mouse.delta.y * rotate_speed;
	fp->look_angle.y -= _input_ctx->mouse.delta.x * rotate_speed;
	fp->look_angle.x = clamp(fp->look_angle.x, -fp->vertical_look_limit, fp->vertical_look_limit);

	fp->rotation = rotation_from_angle_axis(fp->look_angle.y, vec3_up);
	fp->head->rotation = rotation_from_angle_axis(fp->look_angle.x, vec3_right);


	auto move_speed = fp->move_speed * dt;
	if(key_held(Key_Code::A)) fp->acceleration.x = -move_speed;
	else if(key_held(Key_Code::D)) fp->acceleration.x = move_speed;
	
	if(key_held(Key_Code::W)) fp->acceleration.z = -move_speed;
	else if(key_held(Key_Code::S)) fp->acceleration.z = move_speed;

	fp->acceleration = clamp_length(fp->acceleration, move_speed);
	fp->acceleration = mul_dir(fp->acceleration, fp->world_matrix);

	fp->acceleration.y = 0;

	fp->velocity += fp->acceleration;
	fp->velocity *= fp->move_friction;
	fp->acceleration = vec3_zero;
	//fp->position += fp->velocity; 
}

static void
collide_with_mesh(First_Person_Controller* fp, Mesh* mesh, i32 count)
{
	Capsule capsule = {fp->position, fp->radius, fp->height};
	Hit_Info info;

	for(auto i = 0; i < count; ++i)
	{
		Vec3 response = capsule_sweep_mesh(&info, capsule, fp->velocity, mesh);

		if(info.hit)
		{
			auto proj = project(fp->velocity, info.normal);
			fp->velocity -= proj;
		}
	}
}

static void
collide_with_floor(First_Person_Controller* fp, Mesh* mesh)
{
	Hit_Info info;
	Ray ray = {fp->position, vec3_down, 2.0};
	if(ray_mesh(&info, ray, mesh))
	{
		fp->position.y = info.point.y + (fp->height * 0.5);
	}
}