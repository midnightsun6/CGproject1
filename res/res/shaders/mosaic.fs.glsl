#version 430 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float mosaicSize;

void main() {
    vec2 uv = TexCoords / mosaicSize;
    uv = floor(uv) * mosaicSize;

    vec4 color = texture(screenTexture, uv);

    FragColor = color;
}
