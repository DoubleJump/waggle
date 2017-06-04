#VERTEX
attribute vec3 position;
attribute vec2 uv;
attribute float radius;
attribute vec4 color;

uniform mat4 mvp;

varying vec2 _uv;
varying float _radius;
varying vec4 _color;

void main()
{ 
	_uv = uv;
	_radius = radius;
	_color = color;
	gl_Position = mvp * vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

varying vec2 _uv;
varying float _radius;
varying vec4 _color;

float rounded_rectangle(vec2 p, vec2 size, float r) 
{
    vec2 d = abs(p) - (size - r);
    return min(max(d.x,d.y), 0.0) + length(max(d,0.0)) - r;
}

void main()
{ 
	vec2 st = _uv - 0.5;
	float d = rounded_rectangle(st, vec2(0.5, 0.5), _radius);
	d = 1.0 - smoothstep(0.0, 0.004, d);

	vec4 result = _color;
	result.a *= d;
	//result.a = 1.0;
    gl_FragColor = result;
}