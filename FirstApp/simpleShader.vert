#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 rColor;
uniform float xOffset;
uniform float yOffset;
uniform int colorIndex;

vec4 colorOptions[] = {
	vec4(1.0, 1.0, 0.0, 1.0),
	vec4(1.0, 2.0, 0.0, 1.0),
	vec4(1.0, 2.0, 1.0, 1.0),
	vec4(1.0, 1.0, 2.0, 1.0),
	vec4(2.0, 1.0, 1.0, 1.0),
	vec4(2.0, 0.0, 2.0, 1.0)
};

void
main()
{
	if (colorIndex != -1) {
		rColor = vColor + colorOptions[colorIndex];
	} else {
		rColor = vColor;
	}

    gl_Position = vPosition;
	gl_Position.x = gl_Position.x + xOffset;
	gl_Position.y = gl_Position.y + yOffset;
}
