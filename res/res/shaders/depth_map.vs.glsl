#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aOffset;

uniform mat4 lightSpace;
uniform mat4 model;

void main() {
    vec3 pos = vec3(aPos.x + aOffset.x, aPos.y, aPos.z + aOffset.y);
    gl_Position = lightSpace * model * vec4(pos, 1.0);
}