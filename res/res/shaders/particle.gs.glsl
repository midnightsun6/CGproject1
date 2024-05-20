#version 430 core

layout (points) in;
layout (points, max_vertices = 64) out;

in VS_OUT {
	vec3 position;
	vec4 color;
	vec3 velocity;
	float lifetime;
	float size;
} gs_in[];

out vec4 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	// Trail length
	const int numPoints = 10;

	for (int i = 0; i < numPoints; i++) {
		if (gs_in[0].lifetime > 0) {
			float factor = float(gs_in[0].lifetime / float(numPoints - 1)) * float(i) / gs_in[0].lifetime;
			vec3 position = gs_in[0].position + gs_in[0].velocity * factor;
			gl_Position = projection * view * model * vec4(position, 1.0);
			gl_PointSize = gs_in[0].size;

			fragColor = gs_in[0].color * (1.0 - factor);
			EmitVertex();
			EndPrimitive();
		}
	}
}