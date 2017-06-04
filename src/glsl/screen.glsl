#VERTEX
attribute vec3 position;
attribute vec2 uv;

varying vec2 _uv;

void main()
{ 
	_uv = uv;
	gl_Position = vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

uniform sampler2D scene;
uniform sampler2D depth;
uniform sampler2D ui;

varying vec2 _uv;

//import src/glsl/components/gamma.glslc

void main()
{ 
	vec4 scene_sample = to_linear(texture2D(scene, _uv));
	float depth_sample = texture2D(depth, _uv).r;

	vec4 ui_sample = texture2D(ui, _uv);
	vec3 rgb = mix(scene_sample.rgb, ui_sample.rgb, ui_sample.a);
	//vec3 rgb = vec3(depth_sample);
    gl_FragColor = vec4(to_gamma(rgb), 1.0);

}