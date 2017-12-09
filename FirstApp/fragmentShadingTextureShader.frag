#version 150

out vec4  fColor;
//in vec4 rColor;
in vec3 fN;
in vec3 fE;
in vec3 fL;
in vec2 fTexCoord;

uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;
uniform vec4 lightSpecular;

uniform vec4 materialDiffuse;
uniform vec4 materialAmbient;
uniform vec4 materialSpecular;
uniform float shininess;

uniform sampler2D texture;

vec4
computeTexture(vec2 coord)
{
	if(coord.x > 1.5 && coord.y > 1.5) {
		return vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		if(coord.x < 0.5 && coord.y < 0.5) {
			return vec4(0.0, 0.0, 1.0, 1.0);
		}
	}
	if(coord.x > 0.5 && coord.y > 0.5 && coord.x < 1.5 && coord.y < 1.5) {
		return vec4(1.0, 0.0, 0.0, 1.0);
	}
	return vec4(1.0, 1.0, 1.0, 1.0);

}

vec4
computeCircle(vec2 coord)
{
	coord -= vec2(0.5, 0.5);
	if( length(coord) > 0.45 && length(coord) < 0.5) {
		return vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		return vec4(1.0, 1.0, 1.0, 1.0);
	}
}


void
main()
{
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 H = normalize(L + E);
	vec4 ambient = lightAmbient * texture2D(texture, fTexCoord);
	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightDiffuse * texture2D(texture, fTexCoord);//computeTexture(fTexCoord);
	//diffuse = Kd * lightDiffuse * computeCircle(fTexCoord);
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightSpecular * texture2D(texture, fTexCoord);
	if(dot(L, N) < 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}
	fColor = ambient + diffuse + specular;
	fColor.a = 1.0;
}
