#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 rColor;
uniform float xOffset;
uniform float yOffset;
uniform int colorIndex;
uniform mat4 ModelView;

void
main()
{
	if (colorIndex != -1) {
		rColor = vColor;
	} else {
		rColor = vColor;
	}

    gl_Position = ModelView * vPosition;
}
