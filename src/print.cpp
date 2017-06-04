static void
log(f32 v)
{
	printf("%f\n", v);
}

static void
log(i32 v)
{
    printf("%i\n", v);
}

static void
log(u32 v)
{
    printf("%u\n", v);
}

static void 
log(Vec2 v)
{
	printf("%f,%f\n", v.x, v.y);
}

static void 
log(Vec3 v)
{
	printf("%f,%f,%f\n", v.x, v.y, v.z);
}

static void 
log(Vec4 v)
{
    printf("%f,%f,%f,%f\n", v.x, v.y, v.z, v.w);
}

static void
to_string(char* r, f32 v)
{
    sprintf(r, "%.2f", v);
}

static void
to_string(char* r, Vec2 v)
{
	sprintf(r, "[%.2f,%2f]", v.x, v.y);
}

static void
to_string(char* r, Vec3 v)
{
	sprintf(r, "[%.2f,%.2f,%.2f]", v.x, v.y, v.z);
}

static void
to_string(char* r, Vec4 v)
{
	sprintf(r, "[%.2f,%.2f,%.2f,%.2f]", v.x, v.y, v.z, v.w);
}

static void
log(Mesh* mesh)
{
    auto vb = &mesh->vertex_buffer;
    auto ib = &mesh->index_buffer;

    printf("Vertex Buffer:\n");
    printf("VBO: %u\n", vb->vbo);
    printf("Stride: %u\n", vb->stride);
    //printf("Count: %zu\n", mesh->vertex_buffer.count);
    //printf("Capacity: %zu\n", mesh->vertex_buffer.capacity);
    //printf("Offset: %zu\n", mesh->vertex_buffer.offset);
    printf("Num Attributes: %i\n", vb->num_attributes);
    
    auto attr = vb->attributes;
    for(auto i = 0; i < vb->num_attributes; ++i)
    {
        printf("Attr Size: %i\n", attr->size);
        printf("Attr Offest: %i\n", attr->offset);
        attr++;
    }

    printf("Index Buffer:\n");
    printf("IBO: %u\n", ib->ibo);
    printf("Count: %zu\n", ib->count);
    //printf("Count: %zu\n", mesh->index_buffer.capacity);
    //printf("Offset: %zu\n", mesh->index_buffer.offset);
}

static void
log(Texture* t)
{
	printf("Bytes per pixel: %i\n", t->bytes_per_pixel);
	printf("Data size: %zu\n", t->data_size);
	printf("Width: %i\n", t->width);
	printf("Height: %i\n", t->height);
}