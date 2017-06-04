#VERTEX
attribute vec3 position;
attribute vec3 normal;

uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 normal_matrix;

varying vec2 _uv;

void main() 
{
	vec4 p = vec4(position, 1.0);
	vec3 e = normalize(vec3(model_view * p));
	vec3 n = normalize(normal_matrix * normal);
	vec3 r = reflect(e,n);

	float m = 2.0 * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
	_uv = r.xy / m + 0.5;

	gl_Position = projection * model_view * p;
}

#FRAGMENT
precision highp float;

uniform sampler2D image;

varying vec2 _uv;

void main() 
{
	vec3 sample = texture2D(image, 1.0-_uv).rgb;
	gl_FragColor = vec4(sample, 1.0);
	//gl_FragColor = vec4(1,1,0,1);
}