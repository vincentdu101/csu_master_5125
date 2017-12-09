#version 150

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;

uniform vec4 uColor;
out vec4 rColor;
//uniform float offsety;
//uniform float offsetx;
uniform mat4 ModelView;


void
main()
{
    gl_Position = ModelView * vPosition;
	rColor = vec4(abs(vNormal), 1.0);//vColor;
	//rColor = vec4(1.0, 0.0, 0.0, 1.0);
}
