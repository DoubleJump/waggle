#VERTEX

attribute vec3 position;

uniform mat4 model_matrix;
uniform mat4 view_proj_matrix;
uniform mat4 lamp_matrix;

varying vec4 _lamp_space_position;

void main(void) 
{
    _lamp_space_position = lamp_matrix * model_matrix * vec4(position, 1.0);
    gl_Position = view_proj_matrix * model_matrix * vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

uniform sampler2D lamp_depth_map;

varying vec4 _lamp_space_position;

float sample_shadow_map(sampler2D shadow_map, vec2 uv, float depth)
{
	float diff = depth - texture2D(shadow_map, uv).r;
	if(diff > 0.001) return depth * depth;
	return 1.0;
}

/*
float sample_shadow_map_linear(sampler2D shadow_map, vec2 uv, float depth, vec2 texel_size)
{
	vec2 pixel_pos = uv / texel_size * 0.5;
	vec2 frac = fract(pixel_pos);
	vec2 texel = (pixel_pos * frac) * texel_size;

	float bl = sample_shadow_map(shadow_map, texel, depth);
	float br = sample_shadow_map(shadow_map, texel + vec2(texel_size.x, 0.0), depth);
	float tl = sample_shadow_map(shadow_map, texel + vec2(0.0, texel_size.y), depth);
	float tr = sample_shadow_map(shadow_map, texel + texel_size, depth);

	float a = mix(lb, tl, frac.y);
	float b = mix(br, tr, frac.y);

	return mix(a,b, frac.x);
}
*/

void main() 
{
    vec3 uv = (_lamp_space_position.xyz / _lamp_space_position.w) * 0.5 + 0.5;
    float illuminated = sample_shadow_map(lamp_depth_map, uv.xy, uv.z);

    gl_FragColor = vec4(illuminated);
}