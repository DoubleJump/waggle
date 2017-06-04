struct Hash_Key
{
	u32 hash;
	memsize index;
};
struct Hash_Key_32
{
	u32 hash;
	i32 index;
};

static u32
djb_hash(const char* str)
{
    u32 hash = 5381;
    i32 c = 0;

    while((c = *str++)) hash = ((hash << 5) + hash) + c;
    
    return hash;
}

static u32
djb_hash(u32 a, u32 b)
{
    u32 hash = 5381;
    hash = ((hash << 5) + hash) + a;
    hash = ((hash << 5) + hash) + b;
    return hash;
}

static u32
wang_hash(u32 u, u32 v, u32 s)
{
	u32 seed = (u * 1664525u + v) + s;
	seed = (seed ^ 61u) ^ (seed >> 16u);
	seed *= 9u;
	seed = seed ^ (seed >> 4u);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15u);
	return seed;
}

static i32
search_hash_keys(Hash_Key* hash_keys, memsize count, char* name)
{
	auto hash = djb_hash(name);
	auto hash_key = hash_keys;
	for(auto i = 0; i < count; ++i)
	{
		if(hash_key->hash == hash)
		{
			return hash_key->index;
		}
		hash_key++;
	}
	return -1;
}
