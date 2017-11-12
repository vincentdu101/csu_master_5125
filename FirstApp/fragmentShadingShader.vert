#version 150

in vec4 vPosition;
in vec4 v2Position;
in vec4 vColor;
in vec3 vNormal;

uniform vec4 uColor;
//out vec4 rColor;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec3 f2L;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 lightOnePosition;
uniform vec4 lightTwoPosition;

void
main()
{
	//fN = vNormal;
	fN = normalize(ModelView * vec4(vNormal, 0.0)).xyz;
	fE = vPosition.xyz;
	fL = lightOnePosition.xyz - (ModelView * vPosition).xyz;
	f2L = lightTwoPosition.xyz - (ModelView * vPosition).xyz;

    gl_Position = Projection * ModelView * vPosition;
}
