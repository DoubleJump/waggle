static void
push_glyph_vertex(Vertex_Buffer* vb, Glyph_Vertex gv)
{
	auto data = (Glyph_Vertex*)vb->data;
	*(data + vb->offset) = gv;
	vb->count++;
	vb->offset++;
	vb->needs_update = true;
}

static void
push_glyph_triangle(Index_Buffer* ib)
{
	u32 i = ib->offset;
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
add_glyph(Text_Mesh* tm, u8 char_code, u8 prev_code, b32 is_last_glyph)
{
	auto style = tm->style;
	auto font = tm->style->font;
	auto scale = style->size;
	auto color = style->color;

	auto glyph = get_glyph(font, char_code);
	auto kerning = get_kerning(font, char_code, prev_code) * scale;
	//printf("%f\n", kerning);

	auto grid_width = 32.0f;
	auto atlas_width = 512.0f;

	//quad dimensions
	auto hh = (grid_width * scale) / 2;
	auto hw = hh;

	Vec4 uv = glyph.uv;
	Vec3 center = tm->pen;	
	center += glyph.horizontal_bearing * scale;
	center.x += (glyph.size.x * 0.5) * scale;
	center.y -= (glyph.size.y * 0.5) * scale;
	//center.z = tm->index * 0.1;

	if(style->pixel_snap)
	{
		center.x = roundf(center.x);
		center.y = roundf(center.y);
		hh = roundf(hh);
		hw = hh;
	}

	//@HACK: stops z fighting on 3d text
	f32 z = (f32)tm->index * 0.00001;

	auto bl = center + Vec3{-hw, -hh, z};
	auto br = center + Vec3{ hw, -hh, z};
	auto tl = center + Vec3{-hw,  hh, z};
	auto tr = center + Vec3{ hw,  hh, z};
	
	auto vb = &tm->mesh->vertex_buffer;
	push_glyph_vertex(vb, {bl, {uv.x, uv.y}, color});
	push_glyph_vertex(vb, {br, {uv.z, uv.y}, color});
	push_glyph_vertex(vb, {tl, {uv.x, uv.w}, color});
	push_glyph_vertex(vb, {tr, {uv.z, uv.w}, color});

	push_glyph_triangle(&tm->mesh->index_buffer);

	f32 ha = (glyph.advance.x * scale) + kerning + style->letter_spacing;

	auto abs_btm = abs(bl.y);
	if(abs_btm > tm->height) tm->height = abs_btm;

	if(is_whitespace(char_code))
	{
		tm->last_white_space_index = tm->index+1;
		tm->last_white_space_px = tm->pen.x;
		tm->last_white_space_advance = ha;
        //printf("px: %f advance: %f\n", tm->pen.x, ha);
	}

	auto bounds = tm->bounds;
    auto data = (Glyph_Vertex*)vb->data;

	//printf("Bounds: %f X: %f\n", bounds.z, tm->pen.x);
	
	if(bounds.z > 0 && 
	   tm->pen.x > bounds.z && 
	   tm->last_white_space_index > tm->last_line_index)
	{
		tm->width = bounds.z;

		// first go back and h align the previous line
        auto line_width = tm->last_white_space_px - bounds.x;
        auto x_offset = -line_width;
        auto h_align = style->horizontal_alignment;
        
        if(h_align == Horizontal_Alignment::CENTER) x_offset /= 2;
        else if(h_align == Horizontal_Alignment::LEFT) x_offset = 0;


        auto start = tm->last_line_index * 4;
        auto end = tm->last_white_space_index * 4;

        for(auto i = start; i < end; ++i) data[i].position.x += x_offset;

        // drop everything from the last_line_index to here down a line and shove to the left

        x_offset = -((tm->last_white_space_px + tm->last_white_space_advance) - tm->bounds.x);
        auto y_offset = -style->line_height;

        if(style->pixel_snap)
        {
        	x_offset = roundf(x_offset);
        	y_offset = roundf(y_offset);
        }

        start = tm->last_white_space_index * 4;
        end = (tm->index+1) * 4;

        for(auto i = start; i < end; ++i)
        {
            data[i].position += Vec3{x_offset, y_offset, 0};
        }

        tm->pen.x += x_offset + ha;
        tm->pen.y -= style->line_height;
        tm->last_line_index = tm->last_white_space_index;
	}
	else
	{
		tm->pen.x += ha;
	}

	tm->index++;

	if(is_last_glyph)
	{
		auto line_width = tm->pen.x - bounds.x;
        auto x_offset = -line_width;
        auto h_align = style->horizontal_alignment;
        
        if(h_align == Horizontal_Alignment::CENTER) x_offset /= 2;
        else if(h_align == Horizontal_Alignment::LEFT) x_offset = 0;

        auto start = tm->last_line_index * 4;
        auto end = tm->index * 4;

        for(auto i = start; i < end; ++i) data[i].position.x += x_offset;

        auto rhs = tm->pen.x + ha;
        if(rhs > tm->width) tm->width = rhs;
        tm->width = abs(tm->width - bounds.x);

        start = tm->index_start;
        auto y_offset = tm->height;
        auto v_align = style->vertical_alignment;

        if(v_align == Vertical_Alignment::CENTER) y_offset /= 2;
        else if(v_align == Vertical_Alignment::TOP) y_offset = 0;

        for(auto i = start; i < end; ++i) data[i].position.y += y_offset;
	}
}

static void
add_text(Text_Mesh* tm, const char* str)
{
	u32 prev = 0;
	u32 c = 0;
	u32 i = 0;
	u32 len = strlen(str);
	while((c = *str++)) 
	{
		add_glyph(tm, c, prev, i == len-1);
		prev = c;
		i++;
	}
}

static void
reset(Text_Mesh* tm)
{
    tm->pen = {0,0,0};
    tm->index = 0;
    tm->bounds = {0,0,0,0};
    tm->width = 0;
    tm->height = 0;
    tm->index_start = 0;
    tm->last_white_space_index = 0;
    tm->last_white_space_px = 0;
    tm->last_white_space_advance = 0;
    tm->last_line_index = 0;
    tm->last_line_px = 0;
    clear_mesh_buffers(tm->mesh);
}

static Text_Mesh*
new_text(Text_Style* style, memsize capacity, Vec4 bounds)
{
	auto tm = alloc(Text_Mesh);
	init_entity((Entity*)tm);
	tm->style = style;

	Vertex_Attribute attributes[] = 
	{
	    Position_Attribute,
	    UV_Attribute,
	    Color_Attribute
	};

	auto mesh = new_mesh();
	set_vertex_attributes(&mesh->vertex_buffer, &attributes[0], ARRAY_COUNT(attributes));
	alloc_vertex_buffer(&mesh->vertex_buffer, capacity * 4);
	alloc_index_buffer(&mesh->index_buffer, capacity * 6);
	tm->mesh = mesh;

	bind_mesh(mesh);
	reset(tm);
	tm->bounds = bounds;
	return tm;
}

static Text_Mesh*
new_text(Text_Style* style, const char* text, Vec4 bounds)
{
	auto len = strlen(text);
	auto tm = new_text(style, len, bounds);
	add_text(tm, text);
	update_mesh(tm->mesh);
	return tm;
}

static Text_Style*
new_text_style(Font* font, Shader* shader, f32 size, Vec4 color)
{
	auto style = alloc(Text_Style);
	style->shader = shader;
	style->font = font;
	style->size = size;
	style->color = color;
	style->horizontal_alignment = Horizontal_Alignment::LEFT;
	style->letter_spacing = 0;
	style->pixel_snap = false;
	return style;
}

static void
draw_text(Text_Mesh* tm, Camera* camera)
{
	auto style = tm->style;
	use_shader(style->shader);
	Mat4 mvp = tm->world_matrix * camera->view_projection;
    set_uniform("mvp", mvp);
	set_uniform("image", style->font->atlas);
	draw_mesh(tm->mesh);
}