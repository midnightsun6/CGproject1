#version 430 core

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

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

void main()
{
    vec3 lightVec = lightPos - FragPos;

    float dist = length(lightVec);
    float a = 0.0003f;
    float b = 0.00002f;
    float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

    vec3 ambient = material.ambient * lightColor * 0.2f;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightVec);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(viewDir + lightDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * lightColor;

    float shadow = calculateShadow(FragPos, norm, lightDir);

    vec3 result;
    if (hasTexture > 0) {
        vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
        result = (ambient + (1.0 - shadow) * inten * (diffuse + specular)) * textureColor;
    } else {
        result = ambient + (1.0 - shadow) * inten * (diffuse + specular);
    }

    FragColor = vec4(result, 1.0);
}
