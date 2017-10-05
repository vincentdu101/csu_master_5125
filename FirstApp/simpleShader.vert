#version 150

in vec4 vPosition;
in vec4 vColor;
out vec4 rColor;
uniform float xOffset;
uniform float yOffset;
uniform int colorIndex;
uniform vec3 theta;

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

mat4 getRotation() {
	
	vec3 angles = radians(theta);
	vec3 c = cos(angles);
	vec3 s = sin(angles);

	mat4 rx = mat4( 1.0, 0.0, 0.0, 0.0,
					0.0, c.x, -s.x, 0.0,
					0.0, s.x, c.x, 0.0,
					0.0, 0.0, 0.0, 1.0 );

	mat4 ry = mat4( c.y, 0.0, s.y, 0.0, 
				  0.0, 1.0, 0.0, 0.0,
				  -s.y, 0.0, c.y, 0.0, 
				  0.0, 0.0, 0.0, 1.0
				);

	mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
					s.z, c.z, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0);
	return rx * ry * rz;
}

void
main()
{
	if (colorIndex != -1) {
		rColor = vColor + colorOptions[colorIndex];
	} else {
		rColor = vColor;
	}

	mat4 rotation = getRotation();

    gl_Position = rotation * vPosition;
	gl_Position.x = gl_Position.x + xOffset;
	gl_Position.y = gl_Position.y + yOffset;
}
