#version 430 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D velocityTexture;
uniform float blurScale;

void main() {
    vec2 velocity = texture(velocityTexture, TexCoords).xy * blurScale;
    vec4 color = texture(colorTexture, TexCoords);

    vec4 finalColor = color;
    const int numSamples = 16;
    for(int i = 1; i <= numSamples; ++i)
    {
        float t = float(i) / float(numSamples);
        vec2 offset = t * velocity;
        finalColor += texture(colorTexture, TexCoords + offset);
    }
    finalColor /= float(numSamples);

    FragColor = finalColor;
}
