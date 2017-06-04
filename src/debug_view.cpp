static void
reset(Debug_View* dv)
{
	reset_gl_draw(dv->ctx);
	dv->pen = dv->position;
}

static void
render(Debug_View* dv, Camera* camera)
{
	render_gl_draw(dv->ctx, camera);
	reset(dv);
}

static Debug_View*
new_debug_view(const char* name, i32 num_components = 32)
{
	auto dv = alloc(Debug_View);
	dv->ctx = new_gl_draw();
	dv->ctx->text->style->size = 0.4;
	dv->name = name;
	dv->size = {200, 0};
	dv->background = {0,0,0,0.95f};
	dv->inactive = {0.1, 0.1, 0.1, 1.0};
	dv->active = {0.15, 0.15, 0.15, 1.0};
	dv->highlight = Vec4{1,1,1,1};
	dv->position = {0,0,0};
	dv->bounce = 0.1f;
	dv->pen = {0,0,0};
	dv->dragging = false;
	dv->visible = true;
	dv->press_lock = false;
	dv->components = alloc_array(Debug_Type*, num_components);
	dv->num_components = 0;
	return dv;
}

static void
new_debug_drop_down(Debug_View* view, const char* name, u32 count, const char** labels, u32* value)
{
	auto r = alloc(Debug_Drop_Down);
	r->type = Debug_Type::DROP_DOWN;
	r->name = name;
	r->count = count;
	for(auto i = 0; i < count; ++i) r->labels[i] = labels[i];
	r->ref = value;
	r->value = 0;
	r->open = false;

	view->components[view->num_components] = (Debug_Type*)r;
    view->num_components++;
}

static void
new_debug_slider(Debug_View* view, const char* name, f32 min, f32 max, f32* value)
{
    auto sl = alloc(Debug_Slider);
    sl->type = Debug_Type::SLIDER;
    sl->name = name;
    sl->min = min;
    sl->max = max;
    sl->ref = value;
    sl->value = 0;
    sl->dragging = false;

    view->components[view->num_components] = (Debug_Type*)sl;
    view->num_components++;
}

static void
new_debug_switch(Debug_View* view, const char* name, b32* value)
{
	auto s = alloc(Debug_Switch);
	s->type = Debug_Type::SWITCH;
	s->name = name;
	view->components[view->num_components] = (Debug_Type*)s;
    view->num_components++;
}

static void
new_debug_value(Debug_View* view, const char* name, f32* value)
{
	auto s = alloc(Debug_Value);
	s->type = Debug_Type::VALUE;
	s->primitive = Float_Primitive::F32;
	s->name = name;
	s->value = value;
	view->components[view->num_components] = (Debug_Type*)s;
    view->num_components++;
}

static void
new_debug_value(Debug_View* view, const char* name, Vec2* value)
{
	auto s = alloc(Debug_Value);
	s->type = Debug_Type::VALUE;
	s->primitive = Float_Primitive::VEC2;
	s->name = name;
	s->value = value;
	view->components[view->num_components] = (Debug_Type*)s;
    view->num_components++;
}

static void
new_debug_value(Debug_View* view, const char* name, Vec3* value)
{
	auto s = alloc(Debug_Value);
	s->type = Debug_Type::VALUE;
	s->primitive = Float_Primitive::VEC3;
	s->name = name;
	s->value = value;
	view->components[view->num_components] = (Debug_Type*)s;
    view->num_components++;
}

static void
new_debug_value(Debug_View* view, const char* name, Vec4* value)
{
	auto s = alloc(Debug_Value);
	s->type = Debug_Type::VALUE;
	s->primitive = Float_Primitive::VEC4;
	s->name = name;
	s->value = value;
	view->components[view->num_components] = (Debug_Type*)s;
    view->num_components++;
}

static void
draw_value(Debug_View* dv, Debug_Value* v)
{
	auto ctx = dv->ctx;
	auto pen = dv->pen;
	f32 pad = 5;
	f32 width = dv->size.x;
	f32 height = 20;

	// draw bg
	ctx->color = dv->background;
	draw_quad(ctx, {pen.x, pen.y, width, -height});

	// draw text
	ctx->color = {0.8,0.8,0.8,1};
	ctx->text->style->horizontal_alignment = Horizontal_Alignment::LEFT;
	ctx->text->bounds = {0,0,0,0};
	pen += Vec3{pad, -(height - pad), 0};
	draw_text(ctx, v->name, pen);

	// draw value
	ctx->text->style->horizontal_alignment = Horizontal_Alignment::RIGHT;
	ctx->text->bounds = {pen.x + (width-pad),0,0,0};
	
	char val[36];
	switch(v->primitive)
	{
		case Float_Primitive::F32:
		{
			to_string(val, *(f32*)v->value);
			draw_text(ctx, val, pen);
			break;
		}
		case Float_Primitive::VEC2:
		{
			to_string(val, *(Vec2*)v->value);
			draw_text(ctx, val, pen);
			break;
		}
		case Float_Primitive::VEC3:
		{
			to_string(val, *(Vec3*)v->value);
			draw_text(ctx, val, pen);
			break;
		}
		case Float_Primitive::QUATERNION:
		case Float_Primitive::VEC4:
		{
			to_string(val, *(Vec4*)v->value);
			draw_text(ctx, val, pen);
			break;
		}
	}
	
	dv->pen.x = dv->position.x;
	dv->pen.y -= height;
}

static void
draw_label(Debug_View* dv, const char* label)
{
	auto ctx = dv->ctx;
	auto pen = dv->pen;
	f32 pad = 5;
	f32 width = dv->size.x;
	f32 height = 20;

	// draw bg
	ctx->color = dv->background;
	draw_quad(ctx, {pen.x, pen.y, width, -height});

	// draw text
	ctx->color = {0.8,0.8,0.8,1};
	ctx->text->style->horizontal_alignment = Horizontal_Alignment::LEFT;
	ctx->text->bounds = {0,0,0,0};
	pen += Vec3{pad, -(height - pad), 0};
	draw_text(ctx, label, pen);
	
	dv->pen.x = dv->position.x;
	dv->pen.y -= height;
}

static void
draw_switch(Debug_View* dv, Debug_Switch* s)
{
	//draw_label(dv, s->name);

	auto ctx = dv->ctx;
	auto pen = dv->pen;
	f32 pad = 5;
	f32 width = dv->size.x;
	f32 height = 20;
	if(s->ref) s->value = *s->ref;

	// draw bg
	ctx->color = dv->background;
	auto bg = Vec4{pen.x, pen.y, width, -height};
	draw_quad(ctx, bg);

	// draw label
	ctx->color = {0.8,0.8,0.8,1.0};
	draw_text(ctx, s->name, {pen.x + pad, pen.y - (height - pad)});

	ctx->color = dv->inactive;
	f32 check_width = 10;
	
	auto hover = point_in_rect(dv->mouse, bg);
	if(hover && dv->m_down) s->value = !s->value;
	if(s->ref) *s->ref = s->value;

	// draw checkbox

	pen.x += (width - pad - check_width);
	pen.y -= pad;
	ctx->color = dv->inactive;
	if(hover) ctx->color = dv->active;
	if(s->value) ctx->color = dv->highlight;
	auto checkbox = Vec4{pen.x, pen.y, check_width,-check_width};
	draw_quad(ctx, checkbox);

	dv->pen.y += bg.w;
}

static void 
draw_slider(Debug_View* dv, Debug_Slider* sl)
{
	auto ctx = dv->ctx;
	auto pen = dv->pen;
	f32 pad = 5;
	f32 height = 20;
	f32 width = dv->size.x;
	if(sl->ref) sl->value = *sl->ref;

	// draw bg
	ctx->color = dv->background;
	auto bg = Vec4{pen.x, pen.y, width, -((height * 2)-pad)};
	draw_quad(ctx, bg);

	// draw label
	ctx->color = {0.8,0.8,0.8,1.0};
	ctx->text->style->horizontal_alignment = Horizontal_Alignment::LEFT;
	draw_text(ctx, sl->name, {pen.x + pad, pen.y - (height - pad)});

	// draw value
	ctx->text->style->horizontal_alignment = Horizontal_Alignment::RIGHT;
	ctx->text->bounds = {pen.x + (width-pad),0,0,0};
	draw_value(ctx, sl->value, {pen.x + width, pen.y - (height - pad)});

	pen.x += pad;
	pen.y -= height;
	f32 bar_width = width - (pad * 2);
	f32 bar_height = 5;
	auto bar = Vec4{pen.x, pen.y, bar_width, -bar_height};

	auto hover = point_in_rect(dv->mouse, bg);
	if(hover && dv->m_down) sl->dragging = true;
	else if(dv->m_released) sl->dragging = false;

	if(sl->dragging || hover) ctx->color = dv->active;
	else ctx->color = dv->inactive;
	draw_quad(ctx, bar);
	
	if(sl->dragging)
	{
		f32 dx = dv->mouse.x - bar.x;
		auto percent = clamp(dx / bar_width, 0.0,1.0);
		sl->value = lerp(sl->min, sl->max, percent);
	}

	ctx->color = dv->highlight;
	auto percent = (sl->value - sl->min) / (sl->max - sl->min);
	bar.z = bar_width * percent; 
	draw_quad(ctx, bar);

	if(sl->ref) *sl->ref = sl->value;

	ctx->text->style->horizontal_alignment = Horizontal_Alignment::LEFT;
	dv->pen.y += bg.w;
}

static void
draw_drop_down(Debug_View* dv, Debug_Drop_Down* dd)
{
	auto ctx = dv->ctx;
	auto pen = dv->pen;
	f32 pad = 5;
	f32 height = 20;
	f32 width = dv->size.x;

	if(dd->ref) dd->value = *dd->ref;

	// draw bg
	ctx->color = dv->background;
	auto bg = Vec4{pen.x, pen.y, width, -((height*2 + pad))};
	draw_quad(ctx, bg);

	// draw label
	ctx->color = {0.8,0.8,0.8,1.0};
	draw_text(ctx, dd->name, {pen.x + pad, pen.y - (height - pad)});

	pen.x += pad;
	pen.y -= height;
	f32 bar_width = width - (pad * 2);
	f32 bar_height = height;
	auto bar = Vec4{pen.x, pen.y, bar_width, -bar_height};

	auto hover = point_in_rect(dv->mouse, bar);
	ctx->color = dv->inactive;
	if(hover)
	{
		ctx->color = dv->active;
		if(dv->m_down) 
		{
			if(!dd->open &! dv->press_lock)
			{
				dd->open = true;
				dv->press_lock = true;
				dv->m_down = false;
			}
		}
	}

	draw_quad(ctx, bar);

	auto current_label = dd->labels[dd->value];	
	ctx->color = {0.8,0.8,0.8,1.0};
	draw_text(ctx, current_label, {pen.x + pad, pen.y - (height - pad)});

	pen.y -= height;
	if(dd->open)
	{
		for(auto i = 0; i < dd->count; ++i)
		{
			ctx->color = dv->inactive;
			auto bar = Vec4{pen.x, pen.y, bar_width, -bar_height};
			auto hover = point_in_rect(dv->mouse, bar);
			if(hover)
			{
				ctx->color = dv->active;
				if(dv->m_down)
				{
					dd->value = i;
				}
			}
			draw_quad(ctx, bar, 0.0, 0.1);
			
			ctx->color = {0.8,0.8,0.8,1.0};
			draw_text(ctx, dd->labels[i], {pen.x + pad, pen.y - (height - pad), 0.1});

			pen.y -= height;
		}

		if(dv->m_down)
		{
			dd->open = false;
			dv->press_lock = false;
			dv->m_down = false;
		}
	}

	if(dd->ref) *dd->ref = dd->value;
	dv->pen.y += bg.w;
}

static void
update(Debug_View* dv)
{
	dv->m_down = key_down(Key_Code::MOUSE_LEFT);
	dv->m_held = key_held(Key_Code::MOUSE_LEFT);
	dv->m_released = key_released(Key_Code::MOUSE_LEFT);
	dv->mouse = engine->input.mouse.position;
	dv->mouse.y = -dv->mouse.y;
	dv->m_delta.x = engine->input.mouse.delta.x;
	dv->m_delta.y = -engine->input.mouse.delta.y;

	auto ctx = dv->ctx;
	auto pad = 5.0f;
	f32 width = dv->size.x;
	f32 height = 20;

	Vec4 bg = {dv->pen.x, dv->pen.y, width, -height};
	auto hover = point_in_rect(dv->mouse, bg);
	if(hover && dv->m_down) dv->dragging = true;
	else if(dv->m_released) dv->dragging = false;

	ctx->color = dv->background;
	if(dv->dragging)
	{
		dv->position += dv->m_delta;
		dv->velocity = dv->position - dv->last_position;
		dv->last_position = dv->position;
	}
	else
	{
		dv->position += dv->velocity;
		dv->velocity *= 0.9f;
	}

	auto rhs = engine->view.z - width;
	auto btm = 1.0-(engine->view.w - abs(dv->size.y));
	if(dv->position.x < 0 || dv->position.x > rhs)
	{
		dv->velocity.x *= -dv->bounce;
	}
	if(dv->position.y > 0 || dv->position.y < btm)
	{
		dv->velocity.y *= -dv->bounce;
	}

	dv->position.x = clamp(dv->position.x, 0.0f, rhs);
	dv->position.y = clamp(dv->position.y, btm, 0.0f);
	dv->pen = dv->position;

	if(hover) ctx->color = dv->inactive;

	auto pen = dv->pen;
	bg = {pen.x, pen.y, width, -height};
	draw_quad(ctx, bg);

	// draw text
	ctx->color = {0.8,0.8,0.8,1};
	ctx->text->style->horizontal_alignment = Horizontal_Alignment::LEFT;
	ctx->text->bounds = {0,0,0,0};
	pen += Vec3{pad, -height + pad, 0};
	draw_text(ctx, dv->name, pen);

	dv->pen.y -= height;

	auto components = dv->components;
	for(auto i = 0; i < dv->num_components; ++i)
	{
		Debug_Type type = *(components[i]);
		switch(type)
		{
			case Debug_Type::SLIDER:
			{
				auto component = (Debug_Slider*)components[i];
				draw_slider(dv, component);
				break;
			}
			case Debug_Type::SWITCH:
			{
				auto component = (Debug_Switch*)components[i];
				draw_switch(dv, component);
				break;
			}
			case Debug_Type::DROP_DOWN:
			{
				auto component = (Debug_Drop_Down*)components[i];
				draw_drop_down(dv, component);
				break;
			}
			case Debug_Type::VALUE:
			{
				auto component = (Debug_Value*)components[i];
				draw_value(dv, component);
				break;
			}
			NO_DEFAULT_CASE
		}
	}
	dv->size.y = dv->pen.y - dv->position.y;
}
