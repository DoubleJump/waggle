#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ASSERT(x) // void using assert.h.
//#define STBI_MALLOC, 
//#define STBI_REALLOC, 
//#define STBI_FREE

#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_SIMD
#define STBI_ONLY_PNG
//#define STBI_NO_PNG
//#define STBI_NO_JPG
//#define STBI_NO_BMP
//#define STBI_NO_PSD
//#define STBI_NO_TGA
//#define STBI_NO_GIF
//#define STBI_NO_HDR
//#define STBI_NO_PIC
//#define STBI_NO_PNM  


#include "libs/stb_image.h"


static void
read_image(Assets* assets, Binary_Reader* reader, Texture_Format format)
{
    auto file_size = read_i32(reader);
	auto name = read_string(reader);

	auto buffer = reader->buffer;
    auto pad = get_padding(reader->alignment, file_size);

    //let Mr Barrett & co take care of this for now
    i32 width;
    i32 height;
    i32 bpp;

    //stbi__vertically_flip_on_load = true;
    u8* pixels = stbi_load_from_memory(reader->buffer, file_size, &width, &height, &bpp, 0);

    auto t = assets->textures.get_next_free(name);
    t->name = name;
	if(bpp == 3) t->format = Texture_Format::RGB;
	if(bpp == 4) t->format = Texture_Format::RGBA;
	t->width = width;
	t->height = height;
	t->num_pixels = width * height;
	t->bytes_per_pixel = bpp;
	t->pitch = width * t->bytes_per_pixel;
	t->data_size = t->num_pixels * bpp;
	t->data = alloc_array(u8, t->num_pixels * bpp);
	copy_memory(pixels, t->data, t->data_size);
	t->compressed = false;
	t->num_mipmaps = 0;
	t->sampler = &default_sampler;

   	stbi_image_free(pixels);
   	/*
    printf("Name: %s\n", name);
    printf("File Size: %i\n", file_size);
	printf("Width: %i\n", width);
	printf("Height: %i\n", height);
	printf("Channels: %i\n", bpp);
	*/
   	reader->buffer = buffer + file_size + pad;
}

/*
static void
read_dds(Assets* assets, Binary_Reader* reader)
{
	// http://msdn.microsoft.com/en-us/library/bb943991.aspx/
	//var dxt = gb.webgl.extensions.WEBGL_compressed_texture_s3tc;
    const u32 DXT1 = 827611204;
   	const u32 DXT5 = 894720068;

	auto name = read_string(reader);
    auto t = assets->textures.get_next_free(name);

	auto start = reader->buffer;
	auto h = read_i32(reader, 31);
	assert(h[0] == 0x20534444, "Invalid magic number in DDS header");
	assert((!h[20]) & 0x4, "Unsupported format, must contain a FourCC code");

    auto height = h[3];
	auto width = h[4];
	auto four_cc = h[21];
	assert(four_cc == DXT1 || four_cc == DXT5, "Invalid FourCC code");
	
	auto block_size = 0;
	auto bpp = 0;
	switch(four_cc)
	{
		case DXT1:
		{
			block_size = 8;
			//t.format = dxt.COMPRESSED_RGBA_S3TC_DXT1_EXT;
			//t.byte_size = dxt.UNSIGNED_BYTE;
			t->format = Texture_Format::DXT1;
			bpp = 1;
			break;
		}
		case DXT5:
		{
			block_size = 16;
			//t.format = dxt.COMPRESSED_RGBA_S3TC_DXT5_EXT;
			//t.byte_size = dxt.UNSIGNED_SHORT;
			t->format = Texture_Format::DXT5;
			bpp = 2;
			break;
		}
	}
	
	auto size = maximum(4, width) / 4 * maximum(4, height) / 4 * block_size;

	reader->buffer = reader->buffer + (h[1] + 4);
    auto pixels = read_bytes(reader, size);
    
    t->width = width;
    t->height = height;
	t->num_pixels = width * height;
    t->bytes_per_pixel = bpp;
	t->pitch = width * bpp;
	t->data_size = t->num_pixels * bpp;
	t->data = alloc_array(u8, t->num_pixels * bpp);
	copy_memory(pixels, t->data, t->data_size);
    t->compressed = true;
    t->sampler = &default_sampler;

    if(h[2] & 0x20000) t->num_mipmaps = maximum(1, h[7]);
    else t->num_mipmaps = 0;
}
*/