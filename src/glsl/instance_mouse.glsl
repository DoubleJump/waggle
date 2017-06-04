#VERTEX
attribute vec3 position;
attribute vec2 uv;
attribute vec3 instance_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 mouse;

void main()
{ 
	vec3 ip = instance_position;

	vec3 delta = ip - mouse;

	float MAX_DIST = 12.0 * 0.06;
	float dist = clamp(length(delta) / MAX_DIST, 0.0, 1.0);
	vec3 offset = normalize(delta) * dist;

	//if(delta.x > delta.y) offset = vec3(dist, 0,0);
	//else offset = vec3(0, dist, 0); 

	mat4 mv = view * model;
	vec4 p = vec4(ip + position + offset, 1.0);

	gl_Position = projection * mv * p;
}

#FRAGMENT
precision highp float;

void main()
{ 
    gl_FragColor = vec4(1,1,1,1);
}