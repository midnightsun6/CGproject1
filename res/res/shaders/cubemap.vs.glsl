#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform vec4 clipPlane;

void main() {
    vec4 worldPos = vec4(aPos, 1.0);
    gl_ClipDistance[0] = dot(worldPos, clipPlane);

    TexCoords = aPos;
    gl_Position = projection * view * vec4(worldPos, 1.0);
}