#VERTEX
attribute vec3 position;
attribute vec2 uv;
attribute vec3 instance_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

varying vec2 _uv;
varying float _offset;
varying vec3 _pos;

vec3 billboard(mat4 mv, vec3 size)
{
	vec3 right = vec3(mv[0][0], mv[1][0], mv[2][0]) * size.x;
	vec3 up = vec3(mv[0][1], mv[1][1], mv[2][1]) * size.y;
	return right + up;
}

float random (in vec2 st) 
{ 
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise(in vec2 st) 
{
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation
    vec2 u = smoothstep(0.0,1.0,f);

    // Mix 4 coorners porcentages
    return mix(a, b, u.x) + 
            (c - a)* u.y * (1.0 - u.x) + 
            (d - b) * u.x * u.y;
}

void main()
{ 
	_uv = uv;

	vec3 pos = instance_position;
	vec2 st = pos.xz;
	st.x += time;
	//st.y -= time;

	float shift = noise(st * 3.0) * 2.0 + 0.5;
	pos.y = shift * 0.2;
	_pos = pos;

	mat4 mv = view * model;
	vec3 dir = billboard(mv, position);

	vec4 p = vec4(pos + dir, 1.0);

	gl_Position = projection * mv * p;
}

#FRAGMENT
precision highp float;

varying vec2 _uv;
varying vec3 _pos;

float rounded_rectangle(vec2 p, vec2 size, float r) 
{
    vec2 d = abs(p) - (size - r);
    return min(max(d.x,d.y), 0.0) + length(max(d,0.0)) - r;
}

void main()
{ 
	vec2 st = _uv - 0.5;
	float d = rounded_rectangle(st, vec2(0.5, 0.5), 0.5);
	d = 1.0 - smoothstep(0.0, 0.004, d);
	
	vec3 a = vec3(1.0,1.0,1.0);
	vec3 b = vec3(0.0,0.0,0.3);
	vec3 rgb = mix(b,a,_pos.y);
    gl_FragColor = vec4(rgb, d);
}