#version 150

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;

uniform vec4 uColor;
out vec4 rColor;

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;
uniform vec4 lightSpecular;
uniform vec4 lightPosition;

uniform vec4 materialDiffuse;
uniform vec4 materialAmbient;
uniform vec4 materialSpecular;
uniform float shininess;

void
main()
{
	vec3 pos = (ModelView * vPosition).xyz;
	
	vec3 L = normalize(lightPosition.xyz - pos);
	vec3 E = normalize(-pos);
	vec3 H = normalize(L + E);

	vec3 N = normalize(ModelView * vec4(vNormal, 0.0)).xyz;

	vec4 ambient = lightAmbient * materialAmbient;

	float Kd = max(dot(L, N), 0.0);
	vec4 diffuse = Kd * lightDiffuse * materialDiffuse;
	float Ks = pow(max(dot(N, H),0.0), shininess);
	vec4 specular = Ks * lightSpecular * materialSpecular;
	if(dot(L, N) < 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

    gl_Position = Projection * ModelView * vPosition;
	rColor = ambient + diffuse + specular;
	rColor.a = 1.0;

	//rColor = vColor;
	//rColor = vec4(1.0, 0.0, 0.0, 1.0);
}
