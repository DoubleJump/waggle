static void
init_entity(Entity* e)
{
	e->position = {0,0,0};
	e->scale = {1,1,1};
	e->rotation = {0,0,0,1};
	e->local_matrix = mat4_identity();
	e->world_matrix = mat4_identity();
	e->parent = NULL;
	e->active = true;
}

static void
rotate(Entity* e, Vec3 r)
{
	e->rotation *= rotation_from_euler(r);
}

static void
add_child(Entity* parent, Entity* child)
{
	b32 found = false;
	auto current_child = parent->children;
	for(auto i = 0; i < MAX_ENTITY_CHILDREN; ++i)
	{
		if(*current_child == NULL)
		{
			*current_child = child;
			found = true;
			break;
		}
		current_child++;
	}
	assert(found, "Parent cannot hold any more children\n")
}

static void
remove_child(Entity* parent, Entity* child)
{
	b32 found = false;
	auto current_child = parent->children;
	for(auto i = 0; i < MAX_ENTITY_CHILDREN; ++i)
	{
		if(*current_child == NULL) continue;
		if((*current_child)->id == child->id)
		{
			*current_child = NULL;
			found = true;
			break;
		}
		current_child++;
	}
	assert(found, "Child not found when removing from parent\n");
}

static void
set_parent(Entity* e, Entity* parent)
{
	if(e->parent && e->parent->id == parent->id) return;
	if(e->parent && parent == NULL) // clearing parent
	{
		remove_child(parent, e);
		e->parent = NULL;
	}
	else if(e->parent != NULL && parent != NULL) // swapping parent
	{
		remove_child(e->parent, e);
		e->parent = parent;
		add_child(parent, e);
	}
	else // setting new parent from null
	{
		e->parent = parent;
		add_child(parent, e);
	}
}

static void
add_to_scene(Entity* e, Scene* scene)
{
	e->id = scene->num_entities;
	scene->entities[scene->num_entities] = e;
	scene->num_entities++;
}

static Entity*
new_entity(Scene* scene)
{
	auto e = alloc(Entity);
	init_entity(e);
	if(scene) add_to_scene(e, scene);
	return e;
}

static Entity*
new_entity(Scene* scene, Entity* parent)
{
	auto e = new_entity(scene);
	set_parent(e, parent);
	return e;
}


static Scene* 
new_scene(u32 MAX_ENTITIES)
{
	auto scene = alloc(Scene);
	scene->entities = alloc_array(Entity*, MAX_ENTITIES);
	scene->num_entities = 0;
	return scene; 
}

static void
update(Transform* t, Transform* parent)
{
	t->local_matrix = compose(t->position, t->scale, t->rotation);
	if(parent) t->world_matrix = t->local_matrix * parent->world_matrix;
	else t->world_matrix = t->local_matrix;
}

static void
update(Entity* e)
{
	if(!e->active) return;

	update((Transform*)e, (Transform*)e->parent);
	
	for(auto i = 0; i < MAX_ENTITY_CHILDREN; ++i) 
	{
		auto child = e->children[i];
		if(child)  update(child);
	}
}

static void
update(Scene* scene)
{
	for(auto i = 0; i < scene->num_entities; ++i)
	{
		auto e = scene->entities[i];
		if(e && e->parent == NULL) update(e);
	}
}

