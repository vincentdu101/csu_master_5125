#version 150

in vec4 vPosition;
in vec3 vColor;
out vec3 Color;

void
main()
{
    gl_Position = vPosition;
	Color = vColor;
}
