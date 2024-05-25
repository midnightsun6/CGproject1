#version 430 core

in vec4 fragColor;
in vec2 TexCoords;

out vec4 outColor;

uniform sampler2D particleTexture;
uniform bool useTexture;

void main() {
	vec4 color = fragColor;
	if (useTexture) {
		color *= texture(particleTexture, TexCoords);
	}
	outColor = color;
}