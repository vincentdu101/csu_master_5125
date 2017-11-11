#version 150

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;

uniform vec4 uColor;
//out vec4 rColor;

out vec3 fN;
out vec3 fE;
out vec3 fL;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 lightPosition;


void
main()
{
	//fN = vNormal;
	fN = normalize(ModelView * vec4(vNormal, 0.0)).xyz;
	fE = vPosition.xyz;
	fL = lightPosition.xyz - (ModelView * vPosition).xyz;

    gl_Position = Projection * ModelView * vPosition;
}
