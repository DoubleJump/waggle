enum struct Allocator_Type
{
	HEAP = 0,
	BUMP = 1,
};
/*
struct Heap_Tag
{
	b32 in_use;
	memsize size;
};
*/
struct Allocator
{
	Allocator_Type type;
	memsize size;
	memsize used;
	u8 alignment;
	void* base;
	u8* current;
};

global_var Allocator* _allocator_ctx = NULL;

#define copy_array(src, dst, type, count)\
{\
auto sp = (type*)src;\
auto dp = (type*)dst;\
for(memsize i = 0; i < count; ++i) *dp++ = *sp++;\
}\

static void
copy_memory(void* src, void* dst, memsize size)
{
	u8* sp = (u8*)src;
	u8* dp = (u8*)dst;
	for(memsize i = 0; i < size; ++i) *dp++ = *sp++;
}

static void
clear_memory(void* base, memsize size)
{
	u8* mem = (u8*)base;
	for(memsize i = 0; i < size; ++i) *mem++ = 0;
}

static void
set_mem_block(Allocator* block, void* base, memsize size, Allocator_Type type, u8 alignment)
{
	block->size = size;
	block->base = base;
	block->used = 0;
	block->current = (u8*)base;
	block->type = type;
	block->alignment = alignment;

	/*
	if(type == Allocator_Type::HEAP)
	{
		Heap_Tag tag = { false, size };
		auto tag_location = (Heap_Tag*)block->current;
		*tag_location = tag;
		block->used += sizeof(Heap_Tag);
	}
	*/
}

static memsize
get_padding(u32 alignment, memsize size)
{
	return (alignment - size % alignment) % alignment;
}

static b32 
check_available_memory(Allocator* block, memsize size)
{
	return (block->used + size) <= block->size;
}

static void
log_memory_usage(Allocator* block)
{
	switch(block->type)
	{
		case Allocator_Type::BUMP:
		{
			printf("Bump Allocator: %zu of %zu bytes used\n", block->used, block->size);
			break;
		}
		/*
		case Allocator_Type::HEAP:
		{
			printf("Heap: %zu of %zu bytes used\n", block->used, block->size);
			break;
		}
		*/
		NO_DEFAULT_CASE
	}
}

static void*
bump_alloc(Allocator* block, memsize size)
{
	size += get_padding(block->alignment, size);

	#ifdef DEBUG
	assert(check_available_memory(block, size),
	"Tried to allocate %zu bytes but only %zu bytes available in block\n", size, block->size - block->used);
	#endif

	auto address = (void*)block->current;
	block->current += size;
	block->used += size;

	assert(address, "Allocation returned null\n");
	return address;
}

static void
reset_allocator(Allocator* block, b32 zero_out)
{
	if(zero_out) clear_memory(block->base, block->size);
	block->current = (u8*)block->base;
	block->used = 0;
}

static void* 
allocate_memory(memsize size)
{
	assert(_allocator_ctx != NULL, "No allocator set\n");

	switch(_allocator_ctx->type)
	{
		case Allocator_Type::BUMP: return bump_alloc(_allocator_ctx, size);
		//case Allocator_Type::HEAP: return heap_alloc(ctx, size);
		NO_DEFAULT_CASE
	}
	return 0;
}

static void
deallocate_memory(void* location, memsize size)
{
	assert(_allocator_ctx != NULL, "No allocator set\n");

	switch(_allocator_ctx->type)
	{
		case Allocator_Type::BUMP: fail("Can't dealloc items from a pool");
		//case Allocator_Type::HEAP: return heap_free(ctx, location, size);
		NO_DEFAULT_CASE
	}
}

/*
static void
reset(Allocator* block, b32 zero_out)
{
	switch(block->type)
	{
		case Allocator_Type::BUMP: return reset_allocator(block, zero_out);
		//case Allocator_Type::HEAP: return heap_reset(block, zero_out);
		NO_DEFAULT_CASE
	}
}
*/

/*
void* memcpy(void *dst, const void *src, memsize len)
{
	memsize i;
	if((uintptr_t)dst % sizeof(long) == 0 &&
	   (uintptr_t)src % sizeof(long) == 0 &&
	    len % sizeof(long) == 0) 
	{
	    long *d = dst;
		const long *s = src;
		memsize n = len / sizeof(long);
	    for(i = 0; i < n; i++) d[i] = s[i];
	}
	else 
	{
	    u8* d = dst;
	    const u8* s = src;
	    for(i = 0; i < len; i++) d[i] = s[i];
	}

	return dst;
}
*/


#define alloc(type) (type*)allocate_memory(sizeof(type))
#define alloc_array(type, count) (type*)allocate_memory((count) * sizeof(type))

#define clear_struct(s, type) deallocate_memory(s, sizeof(type))
#define clear_array(a, type, count) deallocate_memory(a, sizeof(type) * count)