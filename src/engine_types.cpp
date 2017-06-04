// INPUT

enum struct Key_State 
{
    RELEASED = 0,
    UP = 1,
    DOWN = 2,
    HELD = 3,
};

enum struct Key_Code
{
    MOUSE_LEFT = 0,
    MOUSE_RIGHT = 1,
    BACKSPACE = 8,
    TAB = 9,
    ENTER = 13,
    SHIFT = 16,
    CTRL = 17,
    ALT = 18,
    CAPS = 20,
    ESC = 27,
    SPACE = 32,
    LEFT = 37,
    UP = 38,
    RIGHT= 39,
    DOWN = 40,
    ZERO = 48,
    ONE = 49,
    TWO = 50,
    THREE = 51,
    FOUR = 52,
    FIVE = 53,
    SIX = 54,
    SEVEN = 55,
    EIGHT = 56,
    NINE = 57,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
};

struct Mouse
{
    Vec3 position;
    Vec3 last_position;
    Vec3 delta;
    f32 scroll;
    f32 dy;
    f32 ldy;
};

struct Gyro
{
    Vec3 acceleration;
    Vec3 angular_acceleration;
    Vec4 rotation;
    b32 updated;
};

struct Touch
{
    i32 id;
    b32 is_touching;
    Vec3 position;
    Vec3 last_position;
    Vec3 delta;
    b32 updated;
};

const i32 MAX_TOUCHES = 10;
const i32 NUM_KEYBOARD_KEYS = 256;

struct Input
{
    Mouse mouse;
    Gyro gyro;
    Touch touches[MAX_TOUCHES];
    Key_State keys[NUM_KEYBOARD_KEYS];

    b32 has_gryo;
    b32 has_touches;
    b32 cursor_locked;
    b32 prevent_touch_prop;
};

static Input* _input_ctx;

struct Time
{
    f64 start;
    f64 elapsed;
    f64 now;
    f64 last;
    f64 dt;
    f64 scale;
};

struct Cycler
{
    f32 value;
    f32 rate;
    f32 direction;
};