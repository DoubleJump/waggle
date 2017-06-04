static EM_BOOL 
on_focus_gained(int event_type, const EmscriptenFocusEvent* e, void* user_data)
{
	engine->has_focus = true;
	//printf("FOCUS\n");
	//emscripten_resume_main_loop();
	return false;
}

static EM_BOOL 
on_focus_lost(int event_type, const EmscriptenFocusEvent* e, void* user_data)
{
	engine->has_focus = false;
	//printf("BLUR\n");
	//emscripten_pause_main_loop();
	return false;
}

static EM_BOOL 
on_screen_resize(int event_type, const EmscriptenUiEvent* e, void* user_data)
{
	//printf("RESIZE\n");
	return false;
}

static EM_BOOL 
on_scroll(int event_type, const EmscriptenUiEvent* e, void* user_data)
{
	/*
	printf("detail: %ld, document.body.client size: (%d,%d), window.inner size: (%d,%d), scrollPos: (%d, %d)\n",
   e->detail, e->documentBodyClientWidth, e->documentBodyClientHeight,
e->windowInnerWidth, e->windowInnerHeight, e->scrollTop, e->scrollLeft);
	*/

	//NOTE: e->scrollLeft && e->scrollTop seem to be reversed - file a bug report!
	if(e->scrollLeft > 400)
	{
		engine->can_update = false;
	}
	else
	{
		engine->can_update = true;
	}
	return false;
}

static void
hide_cursor()
{
	emscripten_hide_mouse();
}

static Vec3
get_mouse_world(Camera* camera)
{
	Vec3 mouse= engine->input.mouse.position;
	return screen_to_world(camera->view_projection, mouse, engine->view);
}

static void
init_engine() //TODO: options
{
	i32 width = 0;
	i32 height = 0;
	b32 is_full_screen = false;
	
	//emscripten_set_canvas_size(int width, int height);
	emscripten_get_canvas_size(&width, &height, &is_full_screen);
	//printf("Width: %i Height: %i\n", width, height);

	engine = (Engine*)malloc(sizeof(Engine));
	engine->view = {0,0,(f32)width, (f32)height};

	// MEMORY

	engine->memory_size = MEGABYTES(64);
	engine->memory = malloc(engine->memory_size);

	set_mem_block(&engine->persistant_storage, engine->memory, MEGABYTES(32), Allocator_Type::BUMP, 4);
	set_mem_block(&engine->scratch_storage, (u8*)engine->memory + engine->persistant_storage.size, MEGABYTES(2), Allocator_Type::BUMP, 4);
	_allocator_ctx = &engine->persistant_storage;

	// TIME
	
	init_time(&engine->time, emscripten_get_now());

	// BLUR / FOCUS

	emscripten_set_blur_callback(0, 0, 1, on_focus_lost);
  	emscripten_set_focus_callback(0, 0, 1, on_focus_gained);
  	engine->has_focus = true;
  	engine->can_update = true;
	
	engine->pixel_ratio = (f32)emscripten_get_device_pixel_ratio();
	emscripten_set_resize_callback(0, 0, 1, on_screen_resize);
	emscripten_set_scroll_callback(0, 0, 1, on_scroll);

	// WEBGL CONTEXT

	engine->gl_state = init_webgl();
    set_viewport(engine->view);
	
	// INPUT EVENTS

	init_input(&engine->input);
}