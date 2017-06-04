static EM_BOOL 
on_key_down(int event_type, const EmscriptenKeyboardEvent *e, void *data)
{
	/*
	e->key
	e->code
	e->location 
    e->ctrlKey ? " CTRL" = "", e->shiftKey ? " SHIFT" = "", e->altKey ? " ALT" = "", e->metaKey ? " META" = "", 
	e->repeat, 
	e->locale 
	e->charValue
	e->charCode
	e->keyCode
	e->which

	printf("Code: %s\n", e->code);
	printf("Key: %s\n", e->key);
	printf("Char Code: %lu\n", e->charCode);
	printf("Keycode: %lu\n", e->keyCode);
	*/

	auto kc = e->keyCode;
	if(_input_ctx->keys[kc] != Key_State::HELD) 
	{
		_input_ctx->keys[kc] = Key_State::DOWN;
	}

	return false;
}

static EM_BOOL 
on_key_up(int event_type, const EmscriptenKeyboardEvent *e, void *data)
{
	auto kc = e->keyCode;
	if(_input_ctx->keys[kc] != Key_State::RELEASED) 
	{
		_input_ctx->keys[kc] = Key_State::UP;
	}
	
	return false;
}

// MOUSE

static void
lock_cursor()
{
	_input_ctx->cursor_locked = true;
	emscripten_request_pointerlock(0, true);
}

static void
unlock_cursor()
{
	_input_ctx->cursor_locked = false;
	emscripten_exit_pointerlock();
}

static void
toggle_cursor_lock()
{
	_input_ctx->cursor_locked = !_input_ctx->cursor_locked;
	_input_ctx->mouse.last_position = _input_ctx->mouse.position;
	_input_ctx->mouse.delta = {0,0,0};

	if(_input_ctx->cursor_locked)
	{
		lock_cursor();
		puts("Cursor Lock");
	}
	else 
	{
		unlock_cursor();
		puts("Cursor Free");
	}
}


static EM_BOOL 
on_mouse_up(int event_type, const EmscriptenMouseEvent *e, void *data)
{
	//TODO: left / right button
	_input_ctx->keys[(i32)Key_Code::MOUSE_LEFT] = Key_State::UP;
		
	return false;
}
static EM_BOOL 
on_mouse_down(int event_type, const EmscriptenMouseEvent *e, void *data)
{
	//TODO: left / right button
	_input_ctx->keys[(i32)Key_Code::MOUSE_LEFT] = Key_State::DOWN;
		
	return false;
}

static EM_BOOL 
on_mouse_move(int event_type, const EmscriptenMouseEvent *e, void *data)
{
	if(_input_ctx->cursor_locked)
	{
		Vec3 delta = {(f32)e->movementX, (f32)e->movementY, 0};
		_input_ctx->mouse.position += delta;
	}
	else
	{		
		_input_ctx->mouse.position.x = e->clientX;
		_input_ctx->mouse.position.y = e->clientY;
	}

	return false;
}

static EM_BOOL 
on_mouse_wheel(int event_type, const EmscriptenWheelEvent *e, void *data)
{
	_input_ctx->mouse.dy = e->deltaY;
	return false;
}

static EM_BOOL 
on_touch_start(int event_type, const EmscriptenTouchEvent *e, void *data)
{
	auto n = e->numTouches;
	for(auto i = 0; i < n; ++i)
	{
		auto it = e->touches[i];
		if(!it.isChanged) continue;

		auto t = &_input_ctx->touches[0];
		for(auto j = 0; j < MAX_TOUCHES; ++j)
		{
			if(!t->is_touching)
			{
				f32 x = (f32)it.screenX;
				f32 y = (f32)it.screenY;
				t->id = it.identifier;
				t->is_touching = true;
	  			t->position = {x,y,0};
	  			t->last_position = {x,y,0};
	  			t->delta = {0,0,0};
	  			t->updated = true;

	  			//mouse emulate
	  			_input_ctx->mouse.position = t->position;
	  			_input_ctx->mouse.last_position = t->position;
	  			_input_ctx->keys[(i32)Key_Code::MOUSE_LEFT] = Key_State::DOWN;
			}
			t++;
		}
	}
	return _input_ctx->prevent_touch_prop;
}


static EM_BOOL 
on_touch_move(int event_type, const EmscriptenTouchEvent *e, void *data)
{
	auto n = e->numTouches;
	for(auto i = 0; i < n; ++i)
	{
		auto it = e->touches[i];
		if(!it.isChanged) continue;
		auto id = it.identifier;

		auto t = &_input_ctx->touches[0];
		for(auto j = 0; j < MAX_TOUCHES; ++j)
		{
			if(t->id == id) 
			{
				t->is_touching = true;
  				t->position = {(f32)it.screenX, (f32)it.screenY, 0};
  				t->updated = true;
	  			_input_ctx->mouse.position = t->position;

  				//break;
			}
			t++;
		}
	}
	return _input_ctx->prevent_touch_prop;
}

static EM_BOOL 
on_touch_end(int event_type, const EmscriptenTouchEvent *e, void *data)
{
	auto n = e->numTouches;
	for(auto i = 0; i < n; ++i)
	{
		auto it = e->touches[i];
		if(!it.isChanged) continue;

		auto id = it.identifier;

		auto t = &_input_ctx->touches[0];
		for(auto j = 0; j < MAX_TOUCHES; ++j)
		{
			if(t->id == id) 
			{
				t->is_touching = false;
				t->id = -1;
				t->updated = true;
				_input_ctx->keys[(i32)Key_Code::MOUSE_LEFT] = Key_State::UP;
				//break;
			}
			t++;
		}
	}
	return _input_ctx->prevent_touch_prop;
}

static EM_BOOL 
on_pointer_lock_change(int event_type, const EmscriptenPointerlockChangeEvent *e, void *data)
{
	_input_ctx->cursor_locked = e->isActive;
	return false;
}


static void
init_input(Input* input)
{
	emscripten_set_keyup_callback(0, 0, false, on_key_up);
	emscripten_set_keydown_callback(0, 0, false, on_key_down);
	
	emscripten_set_mouseup_callback(0, 0, false, on_mouse_up);
	emscripten_set_mousedown_callback(0, 0, false, on_mouse_down);
	emscripten_set_mousemove_callback(0, 0, false, on_mouse_move);
	emscripten_set_wheel_callback(0, 0, false, on_mouse_wheel);

	emscripten_set_pointerlockchange_callback(0,0, false, on_pointer_lock_change);
	//emscripten_set_scroll_callback(0, 0, 1, on_mouse_scroll);

	input->cursor_locked = false;

	input->prevent_touch_prop = true;
	emscripten_set_touchstart_callback(0, 0, false, on_touch_start);
  	emscripten_set_touchend_callback(0, 0, false, on_touch_end);
  	emscripten_set_touchmove_callback(0, 0, false, on_touch_move);
  	//emscripten_set_touchcancel_callback(0, 0, 1, touch_callback);

  	//TODO gyro
	
	_input_ctx = input;
}

static b32
key_up(Key_Code code)
{
	return _input_ctx->keys[(i32)code] == Key_State::UP;
}
static b32
key_down(Key_Code code)
{
	return _input_ctx->keys[(i32)code] == Key_State::DOWN;
}
static b32
key_held(Key_Code code)
{
	return _input_ctx->keys[(i32)code] == Key_State::HELD;
}
static b32
key_released(Key_Code code)
{
	return _input_ctx->keys[(i32)code] == Key_State::RELEASED;
}

static void
update(Input* input)
{
	Key_State* key = &input->keys[0];
	for(auto i = 0; i < NUM_KEYBOARD_KEYS; ++i)
	{
		if(*key == Key_State::DOWN) *key = Key_State::HELD;
		else if(*key == Key_State::UP) *key = Key_State::RELEASED;
		key++;
	}

	if(input->mouse.dy == input->mouse.ldy)
	{
		input->mouse.scroll = 0;
	}
	else
	{
		input->mouse.scroll = input->mouse.dy;
		input->mouse.ldy = input->mouse.dy;
	}

	input->mouse.delta = input->mouse.position - input->mouse.last_position;
	input->mouse.last_position = input->mouse.position;
	

	auto t = &input->touches[0];
	for(auto i = 0; i < MAX_TOUCHES; ++i)
	{
		if(t->is_touching)
		{
			t->delta = t->position - t->last_position;
			t->last_position = t->position;
		}
		t++;
	}

	/*
	t = &input->touches[0];
	for(auto i = 0; i < MAX_TOUCHES; ++i)
	{
		if(t->is_touching)
		{
			//input->keys[Key_Code::MOUSE_LEFT] = Key_State::HELD;
			input->mouse.position = t->position;
			input->mouse.delta = t->delta;
			break;
		}
		t++;
	}
	*/
}