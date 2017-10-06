#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 rColor;
uniform float xOffset;
uniform float yOffset;
uniform int colorIndex;
uniform mat4 ModelView;

vec4 colorOptions[] = {
	vec4(0.0, 1.0, 0.0, 1.0),
	vec4(0.0, 0.0, 1.0, 1.0),
	vec4(1.0, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, -1.0, -1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, -1.0, -1.0, 1.0),
	vec4(-1.0, 0.0, 0.0, 1.0)
};

void
main()
{
	if (colorIndex != -1) {
		rColor = vColor + colorOptions[colorIndex];
	} else {
		rColor = vColor;
	}

    gl_Position = ModelView * vPosition;
}
