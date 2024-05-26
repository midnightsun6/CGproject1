#version 430 core

in vec4 FragPos;
in vec4 PrevFragPos;

out vec4 FragColor;

void main() {
    vec2 currentPos = FragPos.xy / FragPos.w;
    vec2 prevPos = PrevFragPos.xy / PrevFragPos.w;

    currentPos = currentPos * 0.5 + 0.5;
    prevPos = prevPos * 0.5 + 0.5;

    vec2 velocity = currentPos - prevPos;

    FragColor = vec4(velocity, 0.0, 1.0);
}
