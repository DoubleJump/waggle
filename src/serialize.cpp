enum struct File_Type : i32
{
	ASSET_BUNDLE = 0,
	
};

struct Binary_Reader
{
	u8* base;
	u8* buffer;
	memsize buffer_size;
	u32 alignment;
};

static Binary_Reader
new_binary_reader(void* buffer, memsize size, u32 align = 4)
{
	Binary_Reader reader = {};
    reader.alignment = align;
	reader.base = (u8*)buffer;
	reader.buffer = reader.base;
	reader.buffer_size = size;
	return reader;
}

static i32
read_i32(Binary_Reader* reader)
{
	i32 result = *(i32*)reader->buffer;
	reader->buffer += sizeof(i32);
	return result;
}

static i32*
read_i32(Binary_Reader* reader, memsize count)
{
	i32* result = (i32*)reader->buffer;
	reader->buffer += sizeof(i32) * count;
	return result;
}

static u32
read_u32(Binary_Reader* reader)
{
	u32 result = *(u32*)reader->buffer;
	reader->buffer += sizeof(u32);
	return result;
}

static u32*
read_u32(Binary_Reader* reader, memsize count)
{
	u32* result = (u32*)reader->buffer;
	reader->buffer += sizeof(u32) * count;
	return result;
}

static f32
read_f32(Binary_Reader* reader)
{
	f32 result = *(f32*)reader->buffer;
	reader->buffer += sizeof(f32);
	return result;
}

static f32*
read_f32(Binary_Reader* reader, memsize count)
{
	f32* result = (f32*)reader->buffer;
	reader->buffer += sizeof(f32) * count;
	return result;
}

static u8*
read_bytes(Binary_Reader* reader, memsize count)
{
	u8* result = reader->buffer;
	reader->buffer += count;
	return result;	
}

static char*
read_string(Binary_Reader* reader)
{
    u32 size = read_u32(reader);
    auto pad = get_padding(reader->alignment, size);
    auto result = alloc_array(char, size);
    copy_memory(reader->buffer, result, size);
    reader->buffer += (size + pad);
    return result;
}