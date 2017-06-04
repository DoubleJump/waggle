static Mat4 
perspective_projection(f32 near, f32 far, f32 aspect, f32 fov)
{
    Mat4 m = mat4_identity();

    f32 h = 1 / tanf(radians(fov / 2));
    f32 y = near - far;
    
    m[ 0] = h / aspect;
    m[ 5] = h;
    m[10] = (far + near) / y;
    m[11] = -1;
    m[14] = 2 * (near * far) / y;
    m[15] = 1;

    return m;
}

static Mat4 
ortho_projection(f32 w, f32 h, f32 near, f32 far)
{
    Mat4 m = mat4_identity();
    
    m[ 0] = 2 / w;
    m[ 5] = 2 / h;
    m[10] = -2 / (far - near);
    m[11] = -near / (far - near);
    m[15] = 1;

    return m;
}

static Vec3 
cartesian_to_polar(Vec3 c)
{
    float radius = length(c);
    return
    {
        atan2f(c.y, c.x),
        acosf(2 / radius),
        radius
    };
}

static Vec3 
polar_to_cartesian(Vec3 p, f32 radius)
{
    f32 ar = radians(p.x);
    f32 br = radians(p.y);
    return
    {
        radius * cosf(br) * cosf(ar),
        radius * sinf(br),
        radius * cosf(br) * sinf(ar)
    };
}

static Vec3 
lng_lat_to_cartesian(f32 lng, f32 lat, f32 radius)
{
    //Vec3 p = {-lng + 90.0f, lat, 0.0f};
    Vec3 p = {lng, lat, 0};
    return polar_to_cartesian(p, radius);
}

static Vec2 
world_to_screen(Mat4 projection, Vec3 world, Vec4 view)
{
    Vec3 wp = mul_projection(world, projection);
    return
    {
        ((wp.x + 1) / 2) * view.z,
        ((1 - wp.y) / 2) * view.w
    };
}

static Vec3 
screen_to_view(Vec3 point, Vec4 view)
{
    return
    {
        point.x / view.z,
        1 - (point.y / view.w),
        point.z
    };
}

static Vec3 
screen_to_world(Mat4 projection, Vec3 point, Vec4 view)
{
    Vec3 t = 
    {
         2 * point.x / view.z - 1,
        -2 * point.y / view.w + 1,
        point.z
    };

    return mul_projection(t, -projection);
}

static Vec4 
world_camera_rect(Mat4 projection, Vec4 view)
{
    Vec3 bl = { 0,0,0 };
    Vec3 tr = { view.z, view.w, 0 };
    
    Vec3 blw = screen_to_world(projection, bl, view);
    Vec3 trw = screen_to_world(projection, tr, view);

    return
    {
        0,0,
        trw.x - blw.x,
        trw.y - blw.y
    };
}