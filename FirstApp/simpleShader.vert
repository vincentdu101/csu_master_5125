#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 rColor;
uniform mat4 ModelView;
uniform vec3 mainColor;

void
main()
{
	rColor = vColor + vec4(mainColor, 1.0);
    gl_Position = ModelView * vPosition;
}
