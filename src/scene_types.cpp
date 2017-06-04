#define MAX_ENTITY_CHILDREN 16

enum struct Entity_Type
{
	DEFAULT = 0,
	CAMERA = 1,
};

struct Transform
{
    Vec3 position;
    Vec3 scale;
    Vec4 rotation;
    Mat4 local_matrix;
    Mat4 world_matrix;
};

struct Entity : Transform
{
	u32 id;
	b32 active;
	Entity_Type entity_type;
	Entity* parent;
	Entity* children[MAX_ENTITY_CHILDREN];
};

struct Camera : Entity
{
	Mat4 projection;
	Mat4 view;
	Mat4 view_projection;
	Mat4 world_to_screen;
	Mat4 screen_to_world;
	Mat3 normal_matrix;
	i32 mask;
	f32 aspect;
	f32 near;
	f32 far;
	f32 fov;
	f32 size;
	b32 orthographic;
};

struct Text_Mesh : Entity
{
    Text_Style* style;
    Mesh* mesh;
    i32 index;
    Vec3 pen;
    Vec4 bounds;
    f32 width;
    f32 height;
    u32 index_start;
    u32 last_line_index;
    u32 last_white_space_index;
    f32 last_white_space_px;
    f32 last_white_space_advance;
    f32 last_line_px;
};

struct Scene
{
	Vec3 position;
	Vec3 scale;
	Vec4 rotation;
	Mat4 matrix;
	Entity** entities;
	u32 num_entities;
};

struct GL_Draw
{
    Vec4 color;
    Mat4 matrix;
    Shader* line_shader;
    Shader* triangle_shader;
    Mesh* lines;
    Mesh* triangles;
    Text_Style* text_style;
    Text_Mesh* text;
};


struct Engine
{
    void* memory;
    memsize memory_size;
    Allocator persistant_storage;
    Allocator scratch_storage;

    b32 has_focus;
    b32 can_update;
    Vec4 view;
    f32 pixel_ratio;
    GL_State* gl_state;
    
    Time time;
    Input input;
    Assets assets;
};
static Engine* engine;