#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aVelocity;
layout (location = 3) in float aLifetime;
layout (location = 4) in float aSize;
layout (location = 5) in int aTrailCount;

out VS_OUT {
    vec3 position;
    vec4 color;
    vec3 velocity;
    float lifetime;
    float size;
    int trailCount;
} vs_out;

void main() {
    vs_out.position = aPosition;
    vs_out.color = aColor;
    vs_out.velocity = aVelocity;
    vs_out.lifetime = aLifetime;
    vs_out.size = aSize;
    vs_out.trailCount = aTrailCount;
}