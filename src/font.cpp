static b32
is_whitespace(char c)
{
	//TODO: tabs etc
	return c == 32;
}

static Glyph
get_glyph(Font* font, u8 c)
{
	return font->glyphs[c - font->range_start];
}

static f32
get_kerning(Font* font, char a, char b)
{
	if(!font->has_kerning) return 0;

	auto max_tries = font->kerning_table.max_tries;
	auto capacity = font->kerning_table.capacity;
	auto hash_keys = font->kerning_table.keys;
	auto hash = djb_hash(a,b) % capacity;
	
	//printf("Hash:%i\n", hash);
	auto tries = 0;

	while(true)
	{
		auto hash_key = hash_keys[hash];
		if(hash_key.code_point_a == a &&
		   hash_key.code_point_b == b)
		{
			//printf("Key %i\n",hash_key.key);
			return font->kerning_table.values[hash_key.key];
		} 
		hash++;
		tries++;

		if(hash >= capacity) hash = 0;
		if(tries > max_tries) break;
	}

	return 0;
}