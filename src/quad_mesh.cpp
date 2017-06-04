static Mesh*
quad_mesh(Vec3 size, Vec3 offset)
{
	f32 w = size.x / 2;
    f32 h = size.y / 2;
    f32 d = size.z / 2;

    f32 x = offset.x;
    f32 y = offset.y;
    f32 z = offset.z;

    f32 vertices[] = 
	{
		x-w, y-h, z+d, 0,0, //bl
	    x+w, y-h, z+d, 1,0, //br
	    x+w, y+h, z-d, 1,1, //tr

	    x-w, y-h, z+d, 0,0, //bl
	    x+w, y+h, z-d, 1,1, //tr
	    x-w, y+h, z-d, 0,1  //tl
	};

	Vertex_Attribute attributes[] = 
	{
	    Position_Attribute,
	    UV_Attribute
	};

	Mesh* mesh = alloc(Mesh);
	mesh->use_index_buffer = false;
	set_vertex_attributes(&mesh->vertex_buffer, &attributes[0], ARRAY_COUNT(attributes));
	alloc_vertex_buffer(&mesh->vertex_buffer, ARRAY_COUNT(vertices));
	set_vertex_data(&mesh->vertex_buffer, &vertices[0], ARRAY_COUNT(vertices));
	bind_mesh(mesh);
	update_mesh(mesh);
	return mesh;
}