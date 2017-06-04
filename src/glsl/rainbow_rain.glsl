#VERTEX
attribute vec3 position;
attribute vec2 uv;
attribute vec3 instance_position;
attribute vec3 instance_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
//uniform mat4 mvp;

varying vec4 _color;

vec3 billboard(mat4 mv, vec3 size)
{
	vec3 right = vec3(mv[0][0], mv[1][0], mv[2][0]) * size.x;
	vec3 up = vec3(0, 1.0, 0.0) * size.y;
	return right + up;
}

//import src/glsl/components/random.glslc
//import src/glsl/components/noise.glslc
//import src/glsl/components/hsb.glslc

float clamp_step(float start, float end, float t)
{
	return clamp((t - start) / (end - start), 0.0, 1.0);
}

void main()
{ 
	vec3 pos = instance_position;
	float N = noise(pos.xz * 3.0) * 2.0 + 0.5;
	
	float loop_t = mod(time,N);
	float scale = (N * uv.y) * loop_t * 0.2;
	float t = mix(2.0,0.0, loop_t * N);

	pos.y += t;

	vec3 hsb = instance_color;
	hsb.r = pos.y;
	vec3 rgb = hsb_to_rgb(hsb);

	mat4 mv = view * model;
	vec3 dir = billboard(mv, position);
	dir.y += scale;

	_color = vec4(rgb, 1.0-uv.y);

	gl_Position = projection * mv * vec4(pos + dir, 1.0);
}

#FRAGMENT
precision highp float;

varying vec4 _color;

void main()
{ 
    gl_FragColor = _color;
}