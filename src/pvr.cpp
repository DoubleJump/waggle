

static void
read_pvr(Assets* assets, Binary_Reader* reader)
{
	//https://developer.apple.com/library/ios/samplecode/PVRTextureLoader/Listings/Classes_PVRTexture_m.html#//apple_ref/doc/uid/DTS40008121-Classes_PVRTexture_m-DontLinkElementID_12
	//var s = gb.binary_reader;
	//var pvr = gb.webgl.extensions.pvr;
	auto name = read_string(reader);
    auto t = assets->textures.get_next_free(name);
    auto start = reader->buffer;
	auto header = read_u32(reader, 13);
	auto header_size = header[12];
	
	auto PVRTC_3 = 55727696;
	auto PVRTC_2 = 0x21525650;
	auto version = header[0];

	assert(version === PVRTC_3, "Unsupported PVRTC format");

	/*
	for(var i = 0; i < 13; ++i)
	{
		console.log("Header " + i + ": " + header[i]);
	}
	*/

	t->width = header[7];
	t->height = header[6];
	t->num_mipmaps = header[11];

	auto format = header[2];
	auto block_width = 8;
	auto block_height = 4;
	auto bpp = 0;
	switch(format)
	{
		case 0:
			t.format = pvr.COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			bpp = 2;
		break;
		case 1:
			t.format = pvr.COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			bpp = 2;
		break;
		case 2:
			t.format = pvr.COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			bpp = 4;
			block_width = 4;
		break;
		case 3:
			t.format = pvr.COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			bpp = 4;
			block_width = 4;
		break;
	}

	auto block_size = (block_width * block_height) * t.byte_size / 8;

	//br.offset += header_size; //+52?
	reader->buffer = start + header_size;
	var size = t.width * t.height;
	auto pixels = read_bytes((br.buffer, br.offset, 10);

    t->compressed = true;
    //br.offset += size * t.byte_size;

	//gb.webgl.update_texture(t, gb.default_sampler);
    //return t;
}

static char gPVRTexIdentifier[4] = "PVR!";

 

enum

{

    kPVRTextureFlagTypePVRTC_2 = 24,
    kPVRTextureFlagTypePVRTC_4

};

 

struct _PVRTexHeader
{
    u32 headerLength;
    u32 height;
    u32 width;
    u32 numMipmaps;
    u32 flags;
    u32 dataLength;
    u32 bpp;
    u32 bitmaskRed;
    u32 bitmaskGreen;
    u32 bitmaskBlue;
    u32 bitmaskAlpha;
    u32 pvrTag;
    u32 numSurfs;
};


static void
read_pvr(Asstes* assets, Binary_Reader* reader)
{
    auto name = read_string(reader);
    auto t = assets->textures.get_next_free(name);
    auto start = reader->buffer;

    u32 dataOffset = 0; 
    u32 dataSize = 0;
    u32 blockSize = 0;
    u32 widthBlocks = 0;
    u23 heightBlocks = 0;
    u32 bpp = 4;
    u8 *bytes = NULL;

    auto header = (PVRTexHeader*)buffer;

    u32 flags = header->flags;
    u32 format_flags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;

    if(format_flags == kPVRTextureFlagTypePVRTC_4 || format_flags == kPVRTextureFlagTypePVRTC_2)
    {
        if (format_flags == kPVRTextureFlagTypePVRTC_4)
            _internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
        else if (format_flags == kPVRTextureFlagTypePVRTC_2)
            _internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;

        u32 width = header->width;
        u32 height = header->height;
        u32 dataLength = header->dataLength;

        reader->buffer += sizeof(PVRTexHeader);
        autp pixels = 

        bytes = ((uint8_t *)[data bytes]) + sizeof(PVRTexHeader);
        

        // Calculate the data size for each texture level and respect the minimum number of blocks

        while(dataOffset < dataLength)
        {
            if(format_flags == kPVRTextureFlagTypePVRTC_4)
            {
                blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
                widthBlocks = width / 4;
                heightBlocks = height / 4;
                bpp = 4;
            }
            else
            {
                blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
                widthBlocks = width / 8;
                heightBlocks = height / 4;
                bpp = 2;
            }

            // Clamp to minimum number of blocks

            if(widthBlocks < 2) widthBlocks = 2;
            if(heightBlocks < 2) heightBlocks = 2;

            dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);

            [_imageData addObject:[NSData dataWithBytes:bytes+dataOffset length:dataSize]];

            dataOffset += dataSize;
            width = MAX(width >> 1, 1);
            height = MAX(height >> 1, 1);
        }

}