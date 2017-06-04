using namespace gb;

struct App
{
	Scene* scene;
	Camera* camera;
	Camera* ui_camera;
	Free_Look* free_look;
	Mesh* quad;
	Mesh* screen_quad;

	Text_Style* style;
	Text_Mesh* text_mesh;

	Render_Target* scene_target;
	Render_Target* ui_target;
	Render_Target* aa_target;

	GL_Draw* gl_draw;
	Debug_View* debug_view;

	First_Person_Controller* fp_controller;
	Entity* point_of_interest;

};
static App* app;

static void 
initialize()
{
	// SCENE
	app = alloc(App);
	auto assets = engine->assets;

	app->scene = new_scene(64);

	app->camera = perspective_camera(0.01, 100, 60, engine->view);
	add_to_scene(app->camera, app->scene);

	app->ui_camera = ui_camera(engine->view);
	add_to_scene(app->ui_camera, app->scene);

	app->free_look = new_free_look(app->camera, 0.5,2);
	app->free_look->angle.y = 120.0f;
	//app->free_look->rotation = {0.015058,0.934985,-0.039984,0.352104};
	app->free_look->position = {6.594514,0.170028,-6.810583};
	add_to_scene(app->free_look, app->scene);

	app->screen_quad = quad_mesh({2,2}, {0,0,0});
	app->quad = quad_mesh({1, 1}, {0,0,0});

	app->fp_controller = new_first_person_controller(app->scene);

	app->point_of_interest = new_entity(app->scene);
	app->point_of_interest->position.y = 1.0;
	app->point_of_interest->scale = {0.5,0.5,0.5};

	// GL DRAW
	
	{
		app->gl_draw = new_gl_draw();
	}

    // RENDER TARGETS

	{
		auto width = engine->view.z;
		auto height = engine->view.w;
		app->scene_target = new_render_target(width, height);
		app->ui_target = new_render_target(width, height);
		app->aa_target = new_render_target(width, height);
	}


    // TEXT

    
    {
	    auto style = app->style;
	    auto font = assets.fonts["input_mono"];
	    auto shader = assets.shaders["msdf_text"];

	    style = new_text_style(font, shader, 0.5f, {0.8,0.8,0.8,1});
	    style->line_height = 0.2;
	    style->size = 0.005;
	    style->color = col_white;

	    auto text = "HELLO FROM WEBASSEMBLY this is a very long piece of text to hopefully overflow the line";

	    app->text_mesh = new_text(style, text, {0,0,3,0});
	    app->text_mesh->position = {0,1,0};
	    add_to_scene(app->text_mesh, app->scene);
	}
	
	

	// DEBUG VIEW
	{
		auto dv = new_debug_view("CONTROLS");
		new_debug_switch(dv, "Switch", NULL);
		new_debug_slider(dv, "FOV", 30,90, &app->fp_controller->camera->fov);
		new_debug_value(dv, "CAM", &app->free_look->position);
		new_debug_slider(dv, "NEAR", -1,1,&app->fp_controller->camera->near);
		new_debug_slider(dv, "FAR",  1,100,&app->fp_controller->camera->far);
		new_debug_slider(dv, "OFFSET",  -1,1,&app->fp_controller->camera->position.z);

		app->debug_view = dv; 
	}

	//TODO: improve how samplers work
	auto whitebox = assets.textures["whitebox"];
	whitebox->sampler = &repeat_sampler;
	update_texture(whitebox);

	update(app->scene);
	update(app->camera);

	// create a box that animates on ray hit
}


static void
update()
{
	auto animations = engine->assets.animations;
	auto textures = engine->assets.textures;
	auto meshes = engine->assets.meshes;
	auto dt = engine->time.dt;

	Vec3 last = app->free_look->position;
	update(app->free_look, dt);

	auto ctx = app->gl_draw;	

	/*
	if(key_held(Key_Code::LEFT))app->test_point.x -= dt;
	if(key_held(Key_Code::RIGHT))app->test_point.x += dt;
	if(key_held(Key_Code::UP))app->test_point.y += dt;
	if(key_held(Key_Code::DOWN))app->test_point.y -= dt;
	*/
	/*
	auto fp = app->fp_controller;
	Capsule capsule = {fp->position, fp->radius, fp->height};
	
	ctx->color = col_aqua;
	draw_wire_capsule(ctx, capsule);
	ctx->color = col_white;
	draw_wire_camera(ctx, fp->camera);
	
	auto wall = meshes["walls"];
	auto floor = meshes["floor"]; 
	ctx->color = {0.5,0.5,0.5,1.0};
	//draw_wire_mesh(ctx, wall);

	update(fp, dt);
	collide_with_floor(fp, floor);
	collide_with_mesh(fp, wall, 4);
	fp->position += fp->velocity;

	Hit_Info info;
	Ray ray = create_camera_ray(fp->camera, 20.0);
	if(ray_mesh(&info, ray, wall))
	{
		ctx->color = col_red;
		draw_hit(ctx, info);
	}
	*/

	Hit_Info info;
	Ray ray = create_camera_ray(app->camera, 20.0);
	if(ray_mesh(&info, ray, meshes["cube"], &app->point_of_interest->world_matrix))
	{
		ctx->color = col_sunflower;
		draw_hit(ctx, info);
	}

	update(app->scene);
	update(app->camera);
	update(app->ui_camera);
	update(app->debug_view);
}


static void 
render()
{
	auto dt = engine->time.dt;
	auto shaders = engine->assets.shaders;
	auto textures = engine->assets.textures;
	auto meshes = engine->assets.meshes;

	auto mvp = mat4_identity();
	Camera* camera;

	auto view = engine->view;
	Vec3 mouse = engine->input.mouse.position;
	Vec3 mw;
	mw.x = (mouse.x / view.z) * (64 * 0.06);
	mw.y = ((view.w - mouse.y) / view.w) * (64 * 0.06);
	mw.z = 0;

	enable_depth_testing();
	enable_alpha_blending();

	//camera = app->fp_controller->camera;
	camera = app->camera;

	set_render_target(app->scene_target);
	set_clear_color({0.0,0.0,0.9,1});

	//disable_depth_testing();
	clear_screen();

	/*
	{
		auto shader = shaders["sphere_map"];
		use_shader(shader);
		
		auto model_view = camera->view;
		set_uniform("model_view", model_view);
		set_uniform("projection", camera->projection);
		set_uniform("normal_matrix", camera->normal_matrix);
		set_uniform("image", textures["matcap_white"]);
		draw_mesh(meshes["walls"]);
		draw_mesh(meshes["floor"]);
	}
	*/

	// DRAW SCENERY

	{
		auto shader = shaders["textured"];
		use_shader(shader);
		set_uniform("mvp", camera->view_projection);
		set_uniform("image", textures["lightmap"]);
		draw_mesh(meshes["construct"]);
	}

	// DRAW CUBE

	{
		auto shader = shaders["flat"];
		use_shader(shader);
		mvp = app->point_of_interest->world_matrix * camera->view_projection;
		set_uniform("mvp", mvp);
		draw_mesh(meshes["cube"]);
	}

	// DRAW TEXT
	
	
	{
		//disable_depth_testing();
		draw_text(app->text_mesh, camera);
	}
	

	// GL DRAW
	{
		auto ctx = app->gl_draw;
		render_gl_draw(ctx, camera);
	}

	// AA PASS
	{
		set_render_target(app->aa_target);
		set_clear_color({0,0,0,0});
		clear_screen();
	    use_shader(shaders["fxaa"]);
	    Vec2 view = {engine->view.z, engine->view.w};
	    set_uniform ("resolution", view);
		set_uniform ("inv_resolution", 1.0f / view);
		set_uniform ("texture", app->scene_target->color);
	    draw_mesh(app->screen_quad);
	}

	// UI PASS

	disable_depth_testing();
	set_blend_mode(Blend_Mode::DEFAULT);
	{
		set_render_target(app->ui_target);
		set_clear_color({0,0,0,0});
		clear_screen();
		camera = app->ui_camera;
		render(app->debug_view, camera);
	}
	
	// FINAL PASS
	{
		set_default_render_target();
	    use_shader(shaders["screen"]);
	    //set_uniform("scene", app->scene_target->color);
	    set_uniform("scene", app->aa_target->color);
	    set_uniform("depth", app->scene_target->depth);
	    set_uniform("ui", app->ui_target->color);
	    draw_mesh(app->screen_quad);
	}
	
}
