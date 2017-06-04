#define BOUNDS_CHECK(index, size) assert(index >= 0 && index < size, "Array index out if bounds");

#define ARRAY(type)\
struct type##_Array\
{\
	memsize length;\
	memsize capacity;\
	type* data;\
	type& operator[](int i)\
	{\
		BOUNDS_CHECK(i,length);\
		return data[i];\
	}\
	type##_Array()\
	{\
		length = 0;\
		data = NULL;\
	}\
	type##_Array(memsize count)\
	{\
		reserve(count);\
	}\
	~type##_Array(){}\
	void reserve(memsize count)\
	{\
		capacity = count;\
		data = (type*)allocate_memory(sizeof(type) * count);\
	}\
	/*\
	void resize(memsize new_count)\
	{\
		if(new_count == length) return;\
		auto new_data = (type*)allocate_memory(sizeof(type) * new_count);\
		copy_memory(data, new_data, sizeof(type) * length);\
		capacity = new_count;\
	}\
	*/\
	void add(type item)\
	{\
		assert(capacity > length, "Not enough space in array");\
		data[length] = item;\
		length++;\
	}\
	type* get_next_free()\
	{\
		assert(capacity > length, "Not enough space in array");\
		type* result = &data[length];\
		length++;\
		return result;\
	}\
};\

#define MAP(type)\
struct type##_Map\
{\
	u32* hash_keys;\
	memsize length;\
	memsize capacity;\
	type* data;\
	type* operator[](const char* name)\
	{\
		auto hash = djb_hash(name);\
		for(auto i = 0; i < length; ++i)\
		{\
			auto hash_key = hash_keys[i];\
			if(hash == hash_key)\
			{\
				return &data[i];\
			}\
		}\
		assert(false, "Key not found in map");\
		return &data[0];\
	}\
	type* get_next_free(const char* name)\
	{\
		auto hash = djb_hash(name);\
		hash_keys[length] = hash;\
		auto value = &data[length];\
		length++;\
		return value;\
	}\
	void reserve(memsize count)\
	{\
		hash_keys = alloc_array(u32, count);\
		data = alloc_array(type, count);\
		capacity = count;\
	}\
};\
