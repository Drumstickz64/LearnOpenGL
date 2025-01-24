#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	float ambientStrength = 0.1;
	vec3 ambient = lightColor * ambientStrength;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * diff;

	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(viewPos - FragPos);

	float specularStrength = 0.5;
	float shininess = 32;
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
	vec3 specular = lightColor * specularStrength * spec;

	vec3 result = (diffuse + ambient + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}
