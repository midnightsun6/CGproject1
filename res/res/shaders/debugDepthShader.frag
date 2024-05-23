#version 430 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float screenWidth;
uniform float screenHeight;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
    float depthValue = texture(depthMap, uv).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}
