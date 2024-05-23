#version 430 core

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main() {
	vec3 I = normalize(WorldPos - cameraPos);
	vec3 R = reflect(I, normalize(Normal));
	vec3 reflection = texture(skybox, R).rgb;
	FragColor = vec4(reflection, 1.0);
}