#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 ClipSpace;

out vec4 FragColor;

uniform sampler2D reflectionTexture;
uniform sampler2D normalMap;
uniform sampler2D dudvMap;
uniform float moveFactor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

const float waveStrength = 0.01;
const float shineDamper = 20.0;
const float reflectivity = 0.6;

void main() {
    vec2 ndc = ClipSpace.xy / ClipSpace.w * 0.5 + 0.5;
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

    vec2 distortedTexCoords = TexCoords + texture(dudvMap, TexCoords + vec2(moveFactor)).rg * 0.1;
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    reflectTexCoords += totalDistortion;
    reflectTexCoords = clamp(reflectTexCoords, vec2(0.001, -0.999), vec2(0.999, -0.001));

    vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);

    vec3 normalMapColor = texture(normalMap, distortedTexCoords).rgb;
    vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0));
    
    vec3 I = normalize(FragPos - lightPos);
    vec3 R = reflect(I, normal);
    float specular = max(dot(R, normalize(viewPos - FragPos)), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularLight = lightColor * specular * reflectivity;

    FragColor = mix(reflectionColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularLight, 0.0);
    FragColor.rgb = clamp(FragColor.rgb, 0.0, 1.0);
}
