#version 430 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
};

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform int hasTexture;

uniform sampler2D texture_diffuse1;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float lowIntensity = step(0.2, diff);
    float midIntensity = step(0.5, diff);
    float highIntensity = step(0.8, diff);

    float intensity = lowIntensity * 0.5 + midIntensity * 0.75 + highIntensity * 1.0;

    vec3 ambient = material.ambient * lightColor;
    vec3 diffuse = material.diffuse * diff;
    vec3 specular = material.specular * spec;

    vec3 color = ambient + intensity * (diffuse + specular);
    vec3 result = color;
    if (hasTexture > 0) {
        result *= texture(texture_diffuse1, TexCoords).rgb;
    }

    FragColor = vec4(result, 1.0);
}