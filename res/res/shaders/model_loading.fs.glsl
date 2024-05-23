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
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform Material material;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform int hasTexture;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

float calShadow(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0;

    float bias = max(0.005 * (1.0 - dot(norm, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -2; x <= 2; ++x) {
        for(int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 25.0;

    return shadow;
}

void main()
{    
    vec3 ambient = material.ambient * lightColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * lightColor;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * lightColor;
    
    float shadow = calShadow(FragPosLightSpace, norm, lightDir);

    vec3 result;
    if(hasTexture > 0) {
        vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
        result = (ambient + (1.0 - shadow) * (diffuse + specular)) * textureColor * 5;
    } else {
        result = (ambient + (1.0 - shadow) * (diffuse + specular)) * 1;
    }

    FragColor = vec4(result, 1.0);
}
