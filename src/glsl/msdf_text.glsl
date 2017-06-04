#VERTEX
attribute vec3 position;
attribute vec2 uv;
attribute vec4 color;

uniform mat4 mvp;

varying vec2 _uv;
varying vec4 _color;

void main()
{ 
	_uv = uv;
	_color = color;
	gl_Position = mvp * vec4(position, 1.0);
}

#FRAGMENT
#extension GL_OES_standard_derivatives : enable
precision highp float;

uniform sampler2D image;

varying vec2 _uv;
varying vec4 _color;

//import src/glsl/components/safe_normalize.glslc


float median(vec3 rgb) 
{
    return max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
}


void main() 
{
    float texture_size = 512.0;

    // Convert normalized texcoords to absolute texcoords.
    vec2 uv = _uv * texture_size;
    
    // Calculate derivates
    vec2 dx = dFdx(uv);
    vec2 dy = dFdy(uv);
    
    vec3 sample = texture2D(image, _uv).rgb;
    float sd = median(sample) - 0.5;
    
    // For proper anti-aliasing, we need to calculate signed distance in pixels. We do this using derivatives.
    vec2 gd = safe_normalize(vec2(dFdx(sd), dFdy(sd)));

    vec2 grad = vec2(gd.x * dx.x + gd.y * dy.x, gd.x * dx.y + gd.y * dy.y);
   
    // Apply anti-aliasing.
    const float thickness = 0.125;

    const float normalization = thickness * 0.5 * sqrt(2.0);

    float afwidth = min(normalization * length(grad), 0.5);
    float alpha = smoothstep(-afwidth, afwidth, sd);
   
    // If enabled apply pre-multiplied alpha. Always apply gamma correction.
    float premul = 1.0;
    //const float GAMMA = 3.0;
    vec4 result;
    //result.a = pow(_color.a * alpha, 1.0 / GAMMA);
    result.a = _color.a * alpha;

    result.rgb = _color.rgb;
    //result.rgb = mix(_color.rgb, _color.rgb * _color.a, premul);

    gl_FragColor = result;
}