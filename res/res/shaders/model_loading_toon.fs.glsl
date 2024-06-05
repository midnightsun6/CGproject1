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
uniform samplerCube shadowMap;

uniform float farPlane;

float calculateShadow(vec3 fragPos, vec3 normal, vec3 lightDir)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(shadowMap, fragToLight).r;
    closestDepth *= farPlane;

    float currentDepth = length(fragToLight);
    float bias = max(0.5f * (1.0f - dot(normal, lightDir)), 0.0005f);

    int sampleRadius = 2;
    float offset = 0.02f;
    float shadow = 0.0;
    for (int z = -sampleRadius; z <= sampleRadius; z++)
    {
        for (int y = -sampleRadius; y <= sampleRadius; y++)
        {
            for (int x = -sampleRadius; x <= sampleRadius; x++)
            {
                float pcfDepth = texture(shadowMap, fragToLight + vec3(x, y, z) * offset).r * farPlane;
                if (currentDepth > pcfDepth + bias)
                    shadow += 1.0;
            }
        }
    }
    shadow /= pow((sampleRadius * 2 + 1), 3);

    return shadow;
}

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

    float shadow = calculateShadow(FragPos, norm, lightDir);

    vec3 color = ambient + intensity * (diffuse + specular) * (1.0 - shadow);
    vec3 result = color;
    if (hasTexture > 0) {
        result *= texture(texture_diffuse1, TexCoords).rgb * (1.0 - shadow);
    }

    FragColor = vec4(result, 1.0);
}