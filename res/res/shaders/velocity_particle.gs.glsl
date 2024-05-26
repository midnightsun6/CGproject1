#version 430 core

layout (points) in;
layout (points, max_vertices = 64) out;

in VS_OUT {
    vec3 position;
    vec4 color;
    vec3 velocity;
    float lifetime;
    float size;
    int trailCount;
} gs_in[];

out vec4 FragPos;
out vec4 PrevFragPos;

uniform mat4 model;
uniform mat4 prevModel;
uniform mat4 view;

uniform mat4 prevView;
uniform mat4 projection;
uniform mat4 prevProjection;

void main() {
    const int numPoints = min(gs_in[0].trailCount + 1, 64);
    for (int i = 0; i < numPoints; i++) {
        vec3 currentPosition = gs_in[0].position + gs_in[0].velocity * float(i) / float(numPoints - 1);
        vec3 prevPosition = gs_in[0].position + gs_in[0].velocity * (float(i) - 1.0) / float(numPoints - 1);

        FragPos = projection * view * model * vec4(currentPosition, 1.0);
        PrevFragPos = prevProjection * prevView * prevModel * vec4(prevPosition, 1.0);

        gl_Position = FragPos;
        EmitVertex();
        EndPrimitive();
    }
}
