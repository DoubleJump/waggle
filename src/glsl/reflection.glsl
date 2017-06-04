#VERTEX
attribute vec3 position;

varying vec3 _position;
varying vec3 _normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main()
{ 
	vec4 p = view * vec4(position, 1.0);

	_position = p.xyz;
    _normal = normal_matrix * normalize(position);

	gl_Position = projection * p;
}

#FRAGMENT
precision highp float;

varying vec3 _position;
varying vec3 _normal;

uniform sampler2D env_map;
uniform mat4 inv_view_matrix;

uniform float bias;
uniform float scale;
uniform float power;

#define PI 3.1415926
#define TWO_PI (2.0 * PI)

vec2 env_map_equirect(vec3 N) 
{
	float phi = acos(-N.y);
	float theta = atan(N.x, N.z) + PI;
	return vec2(theta / TWO_PI, phi / PI);
}

//import src/glsl/components/fresnel.glslc

void main()
{ 
	vec3 eye = normalize(-_position);
    vec3 wc_eye = vec3(inv_view_matrix * vec4(eye, 0.0));
    vec3 wc_normal = vec3(inv_view_matrix * vec4(_normal, 0.0));

    float F = fresnel(wc_eye, wc_normal, bias, scale, power);

    vec3 reflection = reflect(-wc_eye, normalize(wc_normal));
    vec4 sample = texture2D(env_map, env_map_equirect(reflection));
    sample.rgb *= (1.0 - F);
    gl_FragColor = sample;
}