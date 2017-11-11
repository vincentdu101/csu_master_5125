#version 150

in vec4 vPosition;
in vec4 vColor;
uniform vec4 uColor;
out vec4 rColor;
//uniform float offsety;
//uniform float offsetx;
uniform mat4 ModelView;


void
main()
{
    gl_Position = ModelView * vPosition;
	rColor = vColor;
	//rColor = vec4(1.0, 0.0, 0.0, 1.0);
}
