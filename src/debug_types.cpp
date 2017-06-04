enum struct Debug_Type : u32
{
    SLIDER = 0,
    SWITCH = 1,
    DROP_DOWN = 2,
    VALUE = 3,
};

struct Debug_Value
{
    Debug_Type type;
    Float_Primitive primitive;
    const char* name;
    void* value;
};

struct Debug_Switch
{
    Debug_Type type;
    const char* name;
    b32* ref;
    b32 value;
};

struct Debug_Slider
{
    Debug_Type type;
    const char* name;
    f32 min;
    f32 max;
    f32* ref;
    f32 value;
    b32 dragging;   
};


struct Debug_Drop_Down
{
    Debug_Type type;
    const char* name;
    const char* labels[16];
    u32 count;
    u32* ref;
    u32 value;
    b32 open;
};


/*
struct Debug_Group
{
    void* components;
    u32 count;
    b32 dragging;
};
*/
struct Debug_View
{
    GL_Draw* ctx;
    const char* name;
    Vec3 position;
    Vec3 last_position;
    Vec3 velocity;
    f32 bounce;

    Vec3 size;
    Vec3 pen;
    Vec4 background;
    Vec4 inactive;
    Vec4 active;
    Vec4 highlight;
    Vec3 mouse;
    Vec3 m_delta;
    b32 m_down;
    b32 m_held;
    b32 m_released;
    b32 press_lock;

    b32 dragging;
    b32 visible;
    Debug_Type** components;
    u32 num_components;
};
