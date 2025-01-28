#version 330 core

#define NUM_POINT_LIGHTS 4

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
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
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	float distanceToLight = distance(fragPos, light.position);
	float attinuation =
		1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);

	ambient *= attinuation;
	diffuse *= attinuation;
	specular *= attinuation;

	return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

	vec3 spotDir = normalize(-light.direction);
	float cosTheta = dot(lightDir, spotDir);
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((cosTheta - light.outerCutoff) / epsilon, 0.0, 1.0);

	float distanceToLight = distance(light.position, fragPos);
	float attinuation =
		1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);

	ambient *= attinuation * intensity;
	diffuse *= attinuation * intensity;
	specular *= attinuation * intensity;

	return ambient + diffuse + specular;
}