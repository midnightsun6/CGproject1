#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aOffset;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    TexCoord = aTexCoord;
    vec3 pos = aPos + aOffset;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}