#VERTEX

attribute vec3 position;
attribute vec2 uv;

varying vec2 _rgbNW;
varying vec2 _rgbNE;
varying vec2 _rgbSW;
varying vec2 _rgbSE;
varying vec2 _rgbM;
varying vec2 _uv;

uniform vec2 resolution;
uniform vec2 inv_resolution;

void main(void) 
{
    _uv = uv;
    gl_Position = vec4(position, 1.0);

    vec2 frag = uv * resolution;
    _rgbNW = (frag + vec2(-1.0, -1.0)) * inv_resolution;
    _rgbNE = (frag + vec2( 1.0, -1.0)) * inv_resolution;
    _rgbSW = (frag + vec2(-1.0,  1.0)) * inv_resolution;
    _rgbSE = (frag + vec2( 1.0,  1.0)) * inv_resolution;
    _rgbM = vec2(frag * inv_resolution);
}

#FRAGMENT
precision highp float;

#define FXAA_REDUCE_MIN (1.0/ 128.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX   8.0

varying vec2 _rgbNW;
varying vec2 _rgbNE;
varying vec2 _rgbSW;
varying vec2 _rgbSE;
varying vec2 _rgbM;
varying vec2 _uv;

uniform vec2 resolution;
uniform vec2 inv_resolution;
uniform sampler2D texture;

void main() 
{
    vec2 frag = _uv * resolution; 

    vec3 rgbNW = texture2D(texture, _rgbNW).xyz;
    vec3 rgbNE = texture2D(texture, _rgbNE).xyz;
    vec3 rgbSW = texture2D(texture, _rgbSW).xyz;
    vec3 rgbSE = texture2D(texture, _rgbSE).xyz;
    vec4 rgba = texture2D(texture, _rgbM);
    vec3 luma = vec3(0.299, 0.587, 0.114);

    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgba.xyz, luma);
    float lumaMIN = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMAX = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float reduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcp = 1.0 / (min(abs(dir.x), abs(dir.y)) + reduce);

    dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * rcp)) * inv_resolution;
    
    vec3 rgbA = 0.5 * (
        texture2D(texture, frag * inv_resolution + dir * (1.0 / 3.0 - 0.5)).xyz +
        texture2D(texture, frag * inv_resolution + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture2D(texture, frag * inv_resolution + dir * -0.5).xyz +
        texture2D(texture, frag * inv_resolution + dir * 0.5).xyz);

    float lumaB = dot(rgbB, luma);
    if ((lumaB < lumaMIN) || (lumaB > lumaMAX))
    {
        gl_FragColor = vec4(rgbA, rgba.a);
    }
    else
    {
        gl_FragColor = vec4(rgbB, rgba.a);
    }
}