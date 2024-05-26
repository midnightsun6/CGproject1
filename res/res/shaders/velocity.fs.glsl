#version 430 core

in vec4 FragPos;
in vec4 PrevFragPos;

out vec4 FragColor;

void main() {
    vec2 velocity = (FragPos.xy / FragPos.w) - (PrevFragPos.xy / PrevFragPos.w);
    FragColor = vec4(velocity, 0.0, 1.0);
    //FragColor = vec4(1.0, 0, 0, 1.0);
}
