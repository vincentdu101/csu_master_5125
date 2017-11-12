#version 150

out vec4  fColor;
//in vec4 rColor;
in vec3 fN;
in vec3 fE;
in vec3 fL;
in vec3 f2L;

uniform vec4 lightOneDiffuse;
uniform vec4 lightOneAmbient;
uniform vec4 lightOneSpecular;

uniform vec4 lightTwoDiffuse;
uniform vec4 lightTwoAmbient;
uniform vec4 lightTwoSpecular;


uniform vec4 materialDiffuse;
uniform vec4 materialAmbient;
uniform vec4 materialSpecular;
uniform float shininess;


vec4 totalCalculatedLightingForFirstLighting() {
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 twoL = normalize(f2L);

	vec3 H = normalize(L + E);
	vec4 ambient = lightOneAmbient * materialAmbient;
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightOneDiffuse * materialDiffuse;
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightOneSpecular * materialSpecular;
	if(dot(L, N) < 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}
	return ambient + diffuse + specular;
}

vec4 totalCalculatedLightingForSecondLighting() {
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(f2L);

	vec3 H = normalize(L + E);
	vec4 ambient = lightTwoAmbient * materialAmbient;
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightTwoDiffuse * materialDiffuse;
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightTwoSpecular * materialSpecular;
	if(dot(L, N) < 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}
	return ambient + diffuse + specular;
}

void
main()
{
	fColor = totalCalculatedLightingForFirstLighting() + totalCalculatedLightingForSecondLighting();
	fColor.a = 1.0;
}
