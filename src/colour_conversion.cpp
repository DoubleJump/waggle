static Vec3
rgb_to_hsb(Vec3 rgb)
{
    Vec3 hsb;

    f32 r = rgb.x;
    f32 g = rgb.y;
    f32 b = rgb.z;

    f32 c_max = maximum(maximum(r,g),b);
    f32 c_min = minimum(minimum(r,g),b);
    f32 delta = c_max - c_min;

    if(delta != 0.0f)
    {
        f32 hue;
        if(r == c_max) hue = (g - b) / delta;
        else
        {
            if(g == c_max) hue = 2.0f + (b - r) / delta;
            else hue = 4.0f + (r - g) / delta;
        }
        hue *= 60.0f;
        if (hue < 0.0f) hue += 360.0f;
        hsb.hue = hue;
    }
    else hsb.hue = 0.0f;

    if(c_max == 0.0f) hsb.sat = 0.0f;
    else hsb.sat = (c_max - c_min) / c_max;
    
    hsb.brightness = c_max;
    hsb.alpha = rgb.a;
    return hsb;
}

static Vec3
hsb_to_RGB(Vec4 hsb)
{
    f32 h = hsb.x;
    f32 s = hsb.y;
    f32 v = hsb.z;

    if(s <= 0.0) return{ v, v, v, hsb.w};
    
    auto hh = h;
    if(hh > 360.0f) hh = 0.0f;
    if(hh == 360.0f) hh = 359.9f; //fix singularity
    hh /= 60.0f;
    i32 i = floor_to_i32(hh);
    f32 ff = hh - i;
    f32 p = v * (1.0f - s);
    f32 q = v * (1.0f - (s * ff));
    f32 t = v * (1.0f - (s * (1.0f - ff)));

    switch(i) 
    {
        case 0: return {v,t,p, hsb.w};
        case 1: return {q,v,p, hsb.w};
        case 2: return {p,v,t, hsb.w};
        case 3: return {p,q,v, hsb.w};
        case 4: return {t,p,v, hsb.w};
        default: return {v,p,q, hsb.w};
    }
}