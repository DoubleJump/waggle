// ASSETS

enum struct Asset_Type : i32
{
    GLSL = 0,
    SCENE = 1,
    FONT = 2,
    PNG = 3,
    CAMERA = 4,
    LAMP = 5,
    MESH = 6,
    MATERIAL = 7,
    ANIMATION = 8,
    ENTITY = 9,
    EMPTY = 10,
    RIG = 11,
    RIG_ACTION = 12,
    CURVE = 13,
    CUBEMAP = 14,
    JPG = 15,
    DDS = 16,
    PVR = 17,
    END = -1
};

MAP(Shader)
MAP(Texture)
MAP(Mesh)
MAP(Font)
MAP(Animation)

struct Assets
{
    b32 loaded;
    Shader_Map shaders;
    Texture_Map textures;
    Mesh_Map meshes;
    Font_Map fonts;
    Animation_Map animations;
};