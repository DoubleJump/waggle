// MESH 

enum struct Mesh_Primitive
{
    TRIANGLES = 0,
    LINES = 1,
    POINTS = 2,
    STRIP = 3,
};

enum struct Buffer_Update_Rate
{
    STATIC = 0,
    DYNAMIC = 1,
    STREAM = 2,
};

struct Vertex_Attribute
{
    u32 hash;
    u32 size;
    b32 normalized;
    memsize offset;

    Vertex_Attribute(){}
    Vertex_Attribute(const char* name, u32 _size, b32 _normalized)
    {
        hash = djb_hash(name);
        size = _size;
        normalized = _normalized;
    }
    ~Vertex_Attribute(){}
};

static Vertex_Attribute 
Position_Attribute = Vertex_Attribute("position", 3, false);

static Vertex_Attribute 
Normal_Attribute = Vertex_Attribute("normal", 3, false);

static Vertex_Attribute 
UV_Attribute = Vertex_Attribute("uv", 2, false);

static Vertex_Attribute 
Color_Attribute = Vertex_Attribute("color", 4, true);

struct Vertex_Buffer
{
    //u32 vao;
    u32 vbo;
    f32* data;
    memsize data_size;

    u32 stride;
    memsize capacity;
    memsize count;
    memsize offset;
    i32 num_attributes;
    Vertex_Attribute* attributes;
    Buffer_Update_Rate update_rate;
    b32 needs_update;
    b32 instanced;
};

struct Index_Buffer
{
    u32 ibo;
    u32* data;
    memsize data_size;
    memsize capacity;
    memsize offset;
    memsize triangle_offset;
    memsize count;
    Buffer_Update_Rate update_rate;
    b32 needs_update;
};

struct Mesh
{
    const char* name;
    Mesh_Primitive primitive;
    Vertex_Buffer vertex_buffer;
    Index_Buffer index_buffer;
    b32 use_index_buffer;
};

struct GL_Line_Vertex
{
    Vec3 position;
    Vec4 color; 
};

struct GL_Quad_Vertex
{
    Vec3 position;
    Vec2 uv;
    f32 radius;
    Vec4 color;
};

struct Glyph_Vertex
{
    Vec3 position;
    Vec2 uv;
    Vec4 color;
};


// SHADER

#define MAX_SHADER_ATTRIBUTES 16
#define MAX_SHADER_UNIFORMS 16
#define SHADER_ATTRIBUTE_NAME_SIZE 64
#define SHADER_UNIFORM_NAME_SIZE 64

struct Shader_Attribute
{
    char name[SHADER_ATTRIBUTE_NAME_SIZE];
    u32 hash;
    u32 location;
    u32 type;
    u32 size;
};
struct Shader_Uniform
{
    char name[SHADER_UNIFORM_NAME_SIZE];
    u32 hash;
    u32 location;
    u32 type;
    u32 size;
    u32 sampler_index;
};

struct Shader
{
    const char* name;

    char* vertex_src;
    char* fragment_src;

    u32 id;
    i32 num_attributes;
    i32 num_uniforms;
    Shader_Attribute attributes[MAX_SHADER_ATTRIBUTES];
    Shader_Uniform uniforms[MAX_SHADER_UNIFORMS];
};

// TEXTURE

enum struct Texture_Format
{
    RGBA = 0,
    RGB = 1,
    DEPTH_16 = 2,
    DEPTH_32 = 3,
    GRAYSCALE = 4,
    STENCIL = 5,
    DXT1 = 6,
    DXT5 = 7,
};

enum struct Texture_Wrap
{
    REPEAT = 0,
    CLAMP = 1,
};

enum struct Texture_Filter
{
    NEAREST = 0,
    LINEAR = 1,
};

struct Sampler
{
    Texture_Wrap s;
    Texture_Wrap t;
    Texture_Filter up;
    Texture_Filter down;
    f32 anisotropy;
};

static Sampler default_sampler = 
{
    Texture_Wrap::CLAMP,
    Texture_Wrap::CLAMP,
    Texture_Filter::LINEAR,
    Texture_Filter::LINEAR,
    1
};
static Sampler point_sampler = 
{
    Texture_Wrap::CLAMP,
    Texture_Wrap::CLAMP,
    Texture_Filter::NEAREST,
    Texture_Filter::NEAREST,
    1
};
static Sampler repeat_sampler = 
{
    Texture_Wrap::REPEAT,
    Texture_Wrap::REPEAT,
    Texture_Filter::LINEAR,
    Texture_Filter::NEAREST,
    0
};

struct RGBA
{
    u8 r,g,b,a;
};

struct Texture
{
    const char* name;

    u32 id;
    u32 hash;
    i32 width;
    i32 height;
    u8* data;
    memsize data_size;
    memsize num_pixels;
    Texture_Format format;
    Sampler* sampler;
    u32 bytes_per_pixel;
    u32 pitch;
    u32 num_mipmaps;
    b32 compressed;
};

// ALPHA BLENDING //

enum struct Blend_Mode
{
    DEFAULT = 0,
    ADD = 1,
    DARKEN = 2,
    LIGHTEN = 3,
    DIFFERENCE = 4,
    MULTIPLY = 5,
    SCREEN = 6,
    INVERT = 7,
    OVERLAY = 8,
};

// DEPH TESTING

enum struct Depth_Mode
{
    LESS = 0,
    GREATER = 1,
    EQUAL = 2,
    NOT_EQUAL = 3,
    LESS_OR_EQUAL = 4,
    GREATER_OR_EQUAL = 5,
    ALWAYS = 6,
    NEVER = 7
};

enum struct Winding_Order
{
    CW = 0,
    CCW = 1
};

// CLEAR MODE

enum Clear_Mode
{
    COLOR = 0x00004000,
    DEPTH =  0x00000100,
    STENCIL = 0x00000400,
};

// RENDER TARGET

struct Render_Target
{
    u32 width;
    u32 height;
    u32 frame_buffer;
    u32 render_buffer;
    Texture* color;
    Texture* depth;
    Texture* stencil;
};

// GL EXTENSIONS

struct GL_Extensions
{
    b32 ANISITROPIC_FILTERING;
    b32 INSTANCE_ARRAYS;
};
static GL_Extensions _gl_extensions;

// GL STATE

struct GL_State
{
    u32 ctx;
    Shader* shader;
    i32 array_buffer;
    i32 element_buffer;
    i32 texture_id;
    i32 frame_buffer;
    i32 render_buffer;
    u32 clear_mode;
    Vec4 clear_color;
    b32 alpha_blending;
    Blend_Mode blend_mode;
    b32 scissor_test;
    b32 face_culling;
    Winding_Order winding_order;
    b32 depth_test;
    b32 depth_write;
    Depth_Mode depth_mode;
    Vec2 depth_range;
    Vec4 view_port;
};
static GL_State _gl_state;


// TEXT

enum struct Horizontal_Alignment 
{
    LEFT = 0,
    CENTER = 1,
    RIGHT = 2,
};

enum struct Vertical_Alignment
{
    TOP = 0,
    CENTER = 1,
    BOTTOM = 2,
};

#pragma pack(push, 1)
struct Glyph
{
    u32 code_point;
    Vec4 uv;
    Vec2 size;
    Vec2 horizontal_bearing;
    Vec2 vertical_bearing;
    Vec2 advance;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Kern_Key
{
    char code_point_a;
    char code_point_b;
    i32 key;
};
#pragma pack(pop)

struct Kerning_Table
{
    memsize count;
    memsize capacity;
    u32 max_tries;
    Kern_Key* keys;
    f32* values;
};

struct Font
{
    const char* name;

    Texture* atlas;

    u32 range_start;
    u32 num_glyphs;
    Glyph* glyphs;

    b32 has_kerning;
    Kerning_Table kerning_table;

    f32 aspect;
    f32 ascent;
    f32 descent;
    f32 row_height;
    f32 x_height;
    f32 cap_height;
    f32 space_advance;
    //f32 tab_advance;
};

struct Text_Style
{
    Font* font;
    Shader* shader;
    Vec4 color;
    f32 size;
    f32 letter_spacing;
    f32 line_height;
    b32 pixel_snap;
    Vertical_Alignment vertical_alignment;
    Horizontal_Alignment horizontal_alignment;
};
