#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aOffset;

out vec4 FragPos;
out vec4 PrevFragPos;

uniform bool isInstanced;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 prevModel;
uniform mat4 prevView;
uniform mat4 prevProjection;

void main() {
    vec4 worldPosition;
    vec4 prevWorldPosition;
    
    if (isInstanced) {
        worldPosition = model * vec4(aPosition + aOffset, 1.0);
        prevWorldPosition = prevModel * vec4(aPosition + aOffset, 1.0);
    } else {
        worldPosition = model * vec4(aPosition, 1.0);
        prevWorldPosition = prevModel * vec4(aPosition, 1.0);
    }

    FragPos = projection * view * worldPosition;
    PrevFragPos = prevProjection * prevView * prevWorldPosition;
    gl_Position = FragPos;
}
