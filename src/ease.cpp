static f32 
ease_out_quad(f32 t) 
{
    return -1.0 * t * (t - 2.0);
}

static f32 
ease_out_cubic(f32 t) 
{
    return (t-1.0) * t * t + 1.0;
}

static f32 
ease_in_cubic(f32 t) 
{
    return t * t * t;
}

static f32 
ease_in_out_cubic(f32 t) 
{
    auto dt = t *= 2;
    if (dt < 1.0) return 0.5 * t * t * t;
    else 
    {
        auto mt = t -= 2;
        return 0.5 * ((mt) * t * t + 2.0);
    }
}

static f32 
ease_in_out_expo(f32 t) 
{
    if(t == 0.0 || t == 1.0) return t;
    auto dt = t * 2;
    if(dt < 1.0) return 0.5 * pow(2.0, 10.0 * (dt - 1.0));
    else return 0.5 * (-pow(2.0, -10.0 * (t - 1.0)) + 2.0);
}