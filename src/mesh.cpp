static void
set_vertex_attributes(Vertex_Buffer* vb, Vertex_Attribute* attributes, i32 count)
{
    assert(vb->attributes == NULL, "Vertex attributes already allocated");
    vb->attributes = alloc_array(Vertex_Attribute, count);
    
    auto src = attributes;
    auto dst = vb->attributes;
    for(auto i = 0; i < count; ++i)
    {
        *dst = *src;
        dst->offset = vb->stride;
        vb->stride += dst->size;
        src++;
        dst++;
    }
    vb->num_attributes = count;
}

static void
alloc_vertex_buffer(Vertex_Buffer* vb, memsize vertex_count)
{
    assert(vb->data == NULL, "Vertex buffer data already allocated, use resize instead");
    assert(vb->stride != 0, "Vertex attributes not set, can't allocate")
    vb->data_size = (vb->stride * vertex_count) * sizeof(f32);
    vb->data = (f32*)allocate_memory(vb->data_size);
    vb->capacity = vertex_count * vb->stride;
    vb->offset = 0;
    vb->count = 0;
}

static void
set_vertex_data(Vertex_Buffer* vb, f32* data, memsize count)
{
    assert(count > 0, "Count cannot be zero!!!\n")
    assert(count <= vb->capacity, "Vertex buffer not large enough for data, resize first\n");

    auto dst = vb->data;
    for(auto i = 0; i < count; ++i)
    {
        *dst++ = *data++;
    }
    vb->count = count / vb->stride;
    vb->needs_update = true;
}

static void
alloc_index_buffer(Index_Buffer* ib, memsize count)
{   
    assert(ib->data == NULL, "Index buffer data already allocated, use resize instead\n");

    ib->offset = 0;
    ib->count = 0;
    ib->data_size = sizeof(u32) * count;
    ib->capacity = count;
    ib->data = alloc_array(u32, count);
}

static void
set_index_data(Index_Buffer* ib, u32* data, memsize count)
{
    assert(count > 0, "Count cannot be zero!!!\n")
    assert(count <= ib->capacity, "Index buffer not large enough for data, resize first\n");

    auto dst = ib->data;
    for(auto i = 0; i < count; ++i)
    {
        *dst++ = *data++;
    }
    ib->count = count;
    ib->needs_update = true;
}

static void
clear_mesh_buffers(Mesh* mesh)
{
    auto vb = &mesh->vertex_buffer;
    auto ib = &mesh->index_buffer;

    auto n = vb->capacity;
    for(auto i = 0; i < n; ++i) vb->data[i] = 0;
    vb->count = 0;
    vb->offset = 0;

    n = ib->capacity;
    for(auto i = 0; i < n; ++i) ib->data[i] = 0;
    ib->count = 0;
    ib->offset = 0;
    ib->triangle_offset = 0;
}

static void
init_vertex_buffer(Vertex_Buffer* vb, Vertex_Attribute* attributes, i32 num_attributes, 
                   f32* vertices, i32 num_vertices)
{
    set_vertex_attributes(vb, attributes, num_attributes);
    alloc_vertex_buffer(vb, num_vertices);
    set_vertex_data(vb, vertices, num_vertices);
}

static Mesh*
new_mesh()
{
    auto m = alloc(Mesh);
    m->use_index_buffer = true;
    m->primitive = Mesh_Primitive::TRIANGLES;
    m->vertex_buffer = {};
    m->index_buffer = {};
    m->vertex_buffer.data = NULL;
    m->index_buffer.data = NULL;
    return m;
}