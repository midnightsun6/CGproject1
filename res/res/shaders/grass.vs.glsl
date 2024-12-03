#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aOffset;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 clipPlane;

void main()
{
    TexCoords = aTexCoords;
    vec3 pos = aPos + aOffset;
    gl_Position = projection * view * model * vec4(pos, 1.0f); 

    vec4 worldPos = vec4(pos, 1.0);
    gl_ClipDistance[0] = dot(worldPos, clipPlane);
}