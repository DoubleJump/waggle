#VERTEX
attribute vec3 position;
attribute vec2 uv;

uniform mat4 mvp;

varying vec2 _uv;

void main()
{ 
	_uv = uv;
	gl_Position = mvp * vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

uniform sampler2D image;

varying vec2 _uv;

void main()
{ 
	vec2 uv = _uv;
	uv.y = 1.0 - uv.y;
	vec4 sample = texture2D(image, uv);
	gl_FragColor = sample; 
}