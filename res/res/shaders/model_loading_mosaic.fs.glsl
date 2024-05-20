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
in vec4 ScreenPos;

out vec4 FragColor;

uniform Material material;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform int hasTexture;

uniform int screenWidth;
uniform int screenHeight;
uniform float blockSize = 10.f;

uniform sampler2D texture_diffuse1;

vec3 calLightColor() {
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
        result = (ambient + diffuse + specular) * 3;
    }

    return result;
}

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() {    
    vec3 color = calLightColor();

    vec2 normScreenCoords = (ScreenPos.xy / ScreenPos.w) / 2.0 + 0.5;
    normScreenCoords = normScreenCoords * vec2(screenWidth, screenHeight);

    normScreenCoords = floor(normScreenCoords / blockSize) * blockSize;
    normScreenCoords = normScreenCoords / vec2(screenWidth, screenHeight);

    float noise = random(normScreenCoords);
    color *= noise;

    FragColor = vec4(color, 1.0);
}