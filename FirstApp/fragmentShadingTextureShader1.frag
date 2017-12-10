#version 150

out vec4  fColor;
//in vec4 rColor;
in vec3 fN;
in vec3 fE;
in vec3 fL;
in vec3 f2L;
in vec2 fTexCoord;

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

uniform sampler2D texture;
uniform sampler2D second;

vec4 computeTexture(vec2 coord) {
	if (coord.x > 1.5 && coord.y > 1.5) {
		return vec4(0.0, 0.0, 0.0, 1.0);
	} else if (coord.x < 0.5 && coord.y < 0.5) {
		return vec4(0.0, 0.0, 1.0, 1.0);
	}

	if (coord.x > 0.5 && coord.y > 0.5 && coord.x < 1.5 && coord.y < 1.5) {
		return vec4(1.0, 0.0, 0.0, 1.0);
	}

	return vec4(1.0, 1.0, 1.0, 1.0);
}

vec4 totalCalculatedLightingForFirstLighting() {
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 twoL = normalize(f2L);

	N = N * (texture2D(texture, fTexCoord).yxz - 0.5) * 2;

	vec3 H = normalize(L + E);
	vec4 ambient = lightOneAmbient * computeTexture(fTexCoord);
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightOneDiffuse * computeTexture(fTexCoord); 
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightOneSpecular * texture2D(second, fTexCoord);
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

	N = N * (texture2D(texture, fTexCoord).yxz - 0.5) * 2;

	vec3 H = normalize(L + E);
	vec4 ambient = lightTwoAmbient * texture2D(second, fTexCoord);
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightTwoDiffuse * computeTexture(fTexCoord);
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightTwoSpecular * texture2D(second, fTexCoord);
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
