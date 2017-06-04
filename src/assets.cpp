static void
bind_assets(Assets* assets)
{
	Shader* shader = assets->shaders.data;
	auto num_shaders = assets->shaders.length;
    //printf("Num shaders %i\n", num_shaders);
	for(auto i = 0; i < num_shaders; ++i)
	{
		bind_shader(shader);
		shader++;
	}

	Mesh* mesh = assets->meshes.data;
	auto num_meshes = assets->meshes.length;
    //printf("Num meshes %i\n", num_meshes);
	for(auto i = 0; i < num_meshes; ++i)
	{
		bind_mesh(mesh);
		update_mesh(mesh);
		//log(mesh);
		mesh++;
	}

	Texture* texture = assets->textures.data;
	auto num_textures = assets->textures.length;
    //printf("Num textures %i\n", num_textures);
	for(auto i = 0; i < num_textures; ++i)
	{
		bind_texture(texture);
		update_texture(texture);
		//log(texture);
		texture++;
	}

    Font* font = assets->fonts.data;
    auto num_fonts = assets->fonts.length;
    for(auto i = 0; i < num_fonts; ++i)
    {
        font->atlas = assets->textures[font->name];
    }
}

static void
read_shader(Assets* assets, Binary_Reader* reader)
{
	auto name = read_string(reader);
    auto shader = assets->shaders.get_next_free(name);
    shader->name = name;
    shader->vertex_src = read_string(reader);
    shader->fragment_src = read_string(reader);
}

static void
read_mesh(Assets* assets, Binary_Reader* reader)
{
	auto name = read_string(reader);
	auto mesh = assets->meshes.get_next_free(name);

    mesh->name = name;
	mesh->use_index_buffer = false;
	mesh->primitive = Mesh_Primitive::TRIANGLES;
	auto vb = &mesh->vertex_buffer;
	auto ib = &mesh->index_buffer; 

	auto vb_size = read_i32(reader);
	auto vb_data = read_f32(reader, vb_size);
	auto ib_size = read_i32(reader);
	
	if(ib_size > 0)
	{
		mesh->use_index_buffer = true;
		auto ib_data = read_u32(reader, ib_size);
		alloc_index_buffer(ib, ib_size);
		set_index_data(ib, ib_data, ib_size);
	}

	vb->num_attributes = read_i32(reader);
	vb->attributes = alloc_array(Vertex_Attribute, vb->num_attributes);
	vb->stride = 0;
	
	auto attr = vb->attributes;
	for(auto i = 0; i < vb->num_attributes; ++i)
	{
		auto attr_name = read_string(reader);
		attr->hash = djb_hash(attr_name);
		attr->size = read_i32(reader);
        attr->normalized = (b32)read_i32(reader);
        attr->offset = vb->stride;
        vb->stride += attr->size;
        attr++;
	}

	auto vertex_count = vb_size / vb->stride;
	alloc_vertex_buffer(vb, vertex_count);
	set_vertex_data(vb, vb_data, vb_size);
}


static void
read_font(Assets* assets, Binary_Reader* reader)
{
    auto asset_size = read_i32(reader);
    auto name = read_string(reader);

    auto buffer = reader->buffer;
    auto pad = get_padding(reader->alignment, asset_size);
    
    auto font = assets->fonts.get_next_free(name);
    font->name = name;
    font->atlas = NULL;
    font->range_start = read_u32(reader);
    font->num_glyphs = read_u32(reader);

    font->glyphs = alloc_array(Glyph, font->num_glyphs);
    copy_array(reader->buffer, font->glyphs, Glyph, font->num_glyphs);
    reader->buffer += (sizeof(Glyph) * font->num_glyphs);

    font->has_kerning = read_i32(reader);

    if(font->has_kerning)
    {
        auto kern_count = read_u32(reader);
        auto capacity = read_u32(reader);

        font->kerning_table.count = kern_count;
        font->kerning_table.capacity = capacity;
        font->kerning_table.keys = alloc_array(Kern_Key, capacity);
        font->kerning_table.values = alloc_array(f32, kern_count);

        copy_array(reader->buffer, font->kerning_table.keys, Kern_Key, capacity);
        reader->buffer += (sizeof(Kern_Key) * capacity);

        copy_array(reader->buffer, font->kerning_table.values, f32, kern_count);
        reader->buffer += (sizeof(f32) * kern_count);
        
        font->kerning_table.max_tries = read_u32(reader);
        font->ascent = read_f32(reader);
        font->descent = read_f32(reader);
        font->space_advance = read_f32(reader);
        font->x_height = read_f32(reader);
        font->cap_height = read_f32(reader);
    }

    reader->buffer = buffer + asset_size + pad;
}

static void
read_animation(Assets* assets, Binary_Reader* reader)
{
    auto target_type = read_i32(reader);
    auto name = read_string(reader);
    auto anim = assets->animations.get_next_free(name);
    anim->t = 0;
    //TODO: is playing etc

    auto target = read_string(reader);

    auto num_curves = read_i32(reader);
    anim->num_curves = num_curves;
    anim->curves = alloc_array(Animation_Curve, num_curves);

    auto num_props = read_i32(reader);
    anim->num_properties = num_props;
    anim->properties = alloc_array(Animation_Property, num_props);

    auto props = anim->properties;
    for(u32 i = 0; i < num_props; ++i)
    {
        auto prop_name = read_string(reader);
        props->hash = djb_hash(prop_name);
        props->type = (Float_Primitive)read_i32(reader);
        props->offset = read_i32(reader);
        //printf("%s: %u\n", prop_name, props->offset);
        props++;
    }

    auto curve = anim->curves;
    for(u32 i = 0; i < num_curves; ++i)
    {
        curve->num_frames = read_i32(reader);
        auto size = sizeof(Keyframe) * curve->num_frames;
        auto frames = read_bytes(reader, size);
        curve->frames = (Keyframe*)allocate_memory(size);
        copy_memory(frames, curve->frames, size);

        curve++;
    }

    //log(anim);
}

static void 
read_scene(Assets* assets, Binary_Reader* reader)
{
    auto file_size = read_i32(reader);
    auto name = read_string(reader);
    auto buffer = reader->buffer;
    auto pad = get_padding(reader->alignment, file_size);

    //printf("SCENE: %s\n", name);

    auto complete = false;
    while(!complete)
    {
        auto import_type = (Asset_Type)read_i32(reader);
        switch(import_type)
        {
            case Asset_Type::MESH: { read_mesh(assets, reader); break; }
            case Asset_Type::ANIMATION: { read_animation(assets, reader); break; }
            /*
            case Asset_Type::CAMERA: { read_camera(ag); break; }
            case Asset_Type::LAMP: { read_lamp(ag); break; }
            case Asset_Type::MATERIAL: { read_material(ag); break; }
            case Asset_Type::EMPTY: { read_transform(ag); break; }
            case Asset_Type::ENTITY: { read_entity(ag); break; }
            case Asset_Type::RIG: { read_rig(ag); break; }
            case Asset_Type::RIG_ACTION: { read_rig_action(ag); break; }
            case Asset_Type::CURVE: { read_curve(ag); break; }
            */
            case Asset_Type::END: { complete = true; break; }
            default: { complete = true; }
        }
    }

    reader->buffer = buffer + file_size + pad;
}

static void 
on_asset_file_loaded(void* arg, void* buffer, int size) 
{
	Assets* assets = (Assets*)arg;

    // ASSETS

    // TODO: read these counts from the asset file
    assets->shaders.reserve(32);
    assets->textures.reserve(32);
    assets->meshes.reserve(32);
    assets->fonts.reserve(16);
    assets->animations.reserve(16);

	Binary_Reader reader = {};
    reader.alignment = 4;
	reader.base = (u8*)buffer;
	reader.buffer = reader.base;
	reader.buffer_size = size;

    auto complete = false;
    while(!complete)
    {
        auto asset_type = (Asset_Type)read_i32(&reader);

        //printf("Type: %i\n", asset_type);
        switch(asset_type)
        {
            case Asset_Type::GLSL: 
            { 
                //printf("SHADER\n");
            	read_shader(assets, &reader);
            	break; 
            }
            case Asset_Type::SCENE:
            {
                read_scene(assets, &reader);
                break;
            }
            case Asset_Type::FONT:
            {
                read_font(assets, &reader);
                break;
            }
            case Asset_Type::PNG:
            {
                read_image(assets, &reader, Texture_Format::RGBA);
                break;
            }
            case Asset_Type::JPG:
            {
            	read_image(assets, &reader, Texture_Format::RGB);
            	break;
            }
            /*
            case Asset_Type::DDS:
            {
            	read_dds(assets, &reader);
            	break;
            }
            */
            case Asset_Type::END: 
            { 
            	complete = true; 
            	break; 
            }
            NO_DEFAULT_CASE
            //default: { complete = true; }
        }
    }

    //printf("END\n");

    bind_assets(assets);
	assets->loaded = true;
}

static void 
on_data_load_error(void *arg) 
{
	printf("Data load error\n");
}

static void
load_asset_file(const char* url, Assets* assets)
{
	assets->loaded = false;

	// Don't know which gives us more control over memory...
	//emscripten_async_wget(url, url, on_file_load, on_file_load_error);
	emscripten_async_wget_data(url, assets, on_asset_file_loaded, on_data_load_error);
}