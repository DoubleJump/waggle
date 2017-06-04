struct Free_Look : Entity
{
	f32 vertical_limit;
	f32 rotate_speed;
	f32 move_speed;
	Vec3 angle;
	Vec3 velocity;
	b32 locked;
};

static Free_Look*
new_free_look(Camera* camera, f32 move_speed, f32 rotate_speed)
{
	auto fl = alloc(Free_Look);
	init_entity(fl);

	camera->position.z = -0.5;
	set_parent(camera, fl);

	fl->position = {0,0.5,0};
	fl->velocity = {0,0,0};
	fl->angle = {0,0,0};
	fl->move_speed = move_speed;
	fl->rotate_speed = rotate_speed;
	fl->vertical_limit = 80;
	fl->active = true;
	fl->locked = true;
	return fl;
}

static void
update(Free_Look* fl, f32 dt)
{
	if(!fl->active) return;

	if(key_down(Key_Code::F))
	{
		fl->locked = !fl->locked;
		toggle_cursor_lock();
	}

	Vec3 accel = {0,0,0};
	auto move_speed = fl->move_speed * dt;
	auto rotate_speed = fl->rotate_speed * dt;

	if(!fl->locked)
	{
		fl->angle.x -= _input_ctx->mouse.delta.y * rotate_speed;
		fl->angle.y -= _input_ctx->mouse.delta.x * rotate_speed;

		fl->angle.x = clamp(fl->angle.x, -fl->vertical_limit, fl->vertical_limit);

		if(key_down(Key_Code::R))
		{
			fl->angle = {0,0,0};
			fl->velocity = {0,0,0};
			fl->position.x = roundf(fl->position.x);
			fl->position.y = roundf(fl->position.y);
			fl->position.z = 0;
		}
		if(key_down(Key_Code::P))
		{
			log(fl->position);
			log(fl->rotation);
		}

		if(key_held(Key_Code::A)) accel.x = -move_speed;
		else if(key_held(Key_Code::D)) accel.x = move_speed;
		
		if(key_held(Key_Code::W)) accel.z = -move_speed;
		else if(key_held(Key_Code::S)) accel.z = move_speed;

		if(key_held(Key_Code::Q)) accel.y = -move_speed;
		else if(key_held(Key_Code::E)) accel.y = move_speed;
	}

	auto rotation = rotation_from_euler(fl->angle);
	fl->rotation = lerp(fl->rotation, rotation, 0.3f);
	//fl->rotation = rotation;

	accel = mul_dir(accel, fl->world_matrix);
	//accel.y = 0;

	fl->velocity += accel;
	fl->velocity *= 0.9;
	fl->position += fl->velocity; 
}
