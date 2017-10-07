#version 120

varying vec4 vPosition;
varying vec4 vColor;
varying vec4 rColor;
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
