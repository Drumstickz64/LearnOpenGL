#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 direction;
	// cos of angles
	float cutoff;
	float outerCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() {
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 viewDir = normalize(viewPos - FragPos);

	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
	
	vec3 spotDir = normalize(-light.direction);
	float cosTheta = dot(lightDir, spotDir);
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((cosTheta - light.outerCutoff) / epsilon, 0.0, 1.0);

	float distanceToLight = distance(light.position, FragPos);
	float attinuation =
		1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);

	ambient *= attinuation * intensity;
	diffuse *= attinuation * intensity;
	specular *= attinuation * intensity;

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);
}
