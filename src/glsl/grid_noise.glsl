#VERTEX
attribute vec3 position;

uniform mat4 mvp;
uniform float time;

//import src/glsl/components/random.glslc
//import src/glsl/components/simplex_noise.glslc


varying float _depth;

void main()
{ 
	vec3 pos = position;
	vec2 st = pos.xy;
	st.x += time;
	//st.y -= time;

	float shift = snoise(st) * 2.0 + 0.5;
	pos.z += shift * 0.2;
	_depth = pos.z;

	gl_Position = mvp * vec4(pos, 1.0);;
}

#FRAGMENT
precision highp float;

varying float _depth;

void main()
{ 
	vec4 result = mix(vec4(0.03,0.0,0.1,1.0), vec4(0.7,0.8,0.0,1.0), _depth);
    gl_FragColor = vec4(result);
}