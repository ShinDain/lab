#version 330 core

in vec3 v2f_normal;
in vec3 v2f_worldPos; 
in vec2 TexCoord;

out vec4 out_color;

uniform vec3 lightAmbient;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 cameraPos;

uniform sampler2D outTexture;

void main(void)
{	
	vec3 ambient = lightAmbient * lightColor;

	vec3 normalVector = normalize(v2f_normal);
	vec3 lightDir = normalize(lightPos - v2f_worldPos);
	float diffuseLight = max(dot(normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;

	int shininess = 128;
	vec3 viewDir = normalize(cameraPos - v2f_worldPos);
	vec3 reflectDir = reflect(-lightDir, normalVector);
	float specularLight = max(dot(viewDir, reflectDir), 0.0);
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;

	out_color = vec4(result, 1.0);
	out_color = texture(outTexture, TexCoord) * out_color;
}