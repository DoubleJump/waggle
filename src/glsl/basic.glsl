#VERTEX
attribute vec3 position;
attribute vec2 uv;
attribute vec4 color;

uniform mat4 mvp;

varying vec2 _uv;
varying vec4 _color;

void main()
{ 
	_uv = uv;
	_color = color;
	gl_Position = mvp * vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

uniform sampler2D image;

varying vec2 _uv;
varying vec4 _color;

void main()
{ 
	vec4 sample = texture2D(image, _uv);
	sample.a = 0.5;
	gl_FragColor = vec4(0.5,0.5,0.5,1.0); 
}