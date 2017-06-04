#VERTEX
attribute vec3 position;
attribute vec4 color;

uniform mat4 mvp;

varying vec4 _color;

void main()
{ 
	_color = color;
	gl_Position = mvp * vec4(position, 1.0);
}

#FRAGMENT
precision highp float;

varying vec4 _color;

void main()
{ 
	gl_FragColor = _color; 
}