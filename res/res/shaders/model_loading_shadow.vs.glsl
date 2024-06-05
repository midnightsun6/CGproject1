#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aOffset;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpace;

void main() {
    vec4 worldPos = model * vec4(aPos + aOffset, 1.0);

    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    TexCoords = aTexCoords;    
    gl_Position = projection * view * worldPos;
}