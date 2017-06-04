static u32
get_bytes_per_pixel(Texture_Format f)
{
	switch(f)
	{
		case Texture_Format::GRAYSCALE: return 1;
		case Texture_Format::DEPTH_16: return 2;
		case Texture_Format::DEPTH_32: return 4;
		case Texture_Format::RGB: return 3;
		case Texture_Format::RGBA: return 4;
		NO_DEFAULT_CASE
	}
	return 0;
}

static Texture* 
new_texture(u32 width, u32 height, Texture_Format format, Sampler* sampler)
{
	Texture* t = alloc(Texture);
	t->format = format;
	t->width = width;
	t->height = height;
	t->num_pixels = width * height;
	t->bytes_per_pixel = get_bytes_per_pixel(format);
	t->pitch = width * t->bytes_per_pixel;
	t->data_size = t->num_pixels * t->bytes_per_pixel;
	t->data = NULL;
	t->compressed = false;
	t->num_mipmaps = 0;
	t->sampler = sampler;
	return t;
}

static void
allocate_texture_memory(Texture* t)
{
	assert(t->data_size > 0, "Texture data size is zero");
	t->data = (u8*)allocate_memory(t->data_size);
}

static Texture*
rgba_texture(u32 width, u32 height, Sampler* sampler)
{
	auto t = new_texture(width, height, Texture_Format::RGBA, sampler);
	return t;
}

static Texture*
depth_texture(u32 width, u32 height)
{
	auto t = new_texture(width, height, Texture_Format::DEPTH_16, &default_sampler);
	return t;
}