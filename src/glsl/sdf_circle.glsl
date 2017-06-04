#VERTEX
attribute vec3 position;
attribute vec2 uv;

uniform mat4 mvp;

varying vec2 _uv;
varying vec4 _color;

void main()
{ 
	_uv = uv;
	gl_Position = mvp * vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

varying vec2 _uv;

uniform float blur;
uniform float radius;
uniform vec4 color;

float circle(vec2 st, float r, float blur)
{
    vec2 dist = st - vec2(0.5);
	return 1.0 - smoothstep(r-(r * blur), r+(r * blur), dot(dist,dist) * blur);
}

//import src/glsl/components/gamma.glslc

void main()
{ 
	float d = circle(_uv, radius, blur);
	vec4 result = color;
	result.a *= d;
	result *= 0.8;
    gl_FragColor = to_gamma(result);
}