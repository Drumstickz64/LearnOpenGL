#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;

void main() {
	FragColor = vec4(min(lightColor * 4.0, 1.0), 1.0);
}