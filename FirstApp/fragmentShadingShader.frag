#version 150

out vec4  fColor;
//in vec4 rColor;
in vec3 fN;
in vec3 fE;
in vec3 fL;

uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;
uniform vec4 lightSpecular;

uniform vec4 materialDiffuse;
uniform vec4 materialAmbient;
uniform vec4 materialSpecular;
uniform float shininess;


void
main()
{
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 H = normalize(L + E);
	vec4 ambient = lightAmbient * materialAmbient;
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightDiffuse * materialDiffuse;
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightSpecular * materialSpecular;
	if(dot(L, N) < 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}
	fColor = ambient + diffuse + specular;
	fColor.a = 1.0;
}
