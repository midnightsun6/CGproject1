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
    
    vec3 result;
    if(hasTexture > 0) {
        vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
        result = (ambient + diffuse + specular) * textureColor * 5;
    } else {
        result = (ambient + diffuse + specular) * 1;
    }

    FragColor = vec4(result, 1.0);
    //FragColor = texture(texture_diffuse1, TexCoords);
}