#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 ClipSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float tiling = 6.0;

void main()
{
    vec3 pos = aPos + aOffset;
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    ClipSpace = projection * view * vec4(FragPos, 1.0);
    gl_Position = ClipSpace;

    TexCoords = vec2(aTexCoords.x / 2.0 + 0.5, aTexCoords.y / 2.0 + 0.5) * tiling;
}
