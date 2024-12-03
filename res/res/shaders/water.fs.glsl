#version 430 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 ClipSpace;

out vec4 FragColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D normalMap;
uniform sampler2D dudvMap;
uniform float moveFactor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

const float waveStrength = 0.005;
const float shineDamper = 20.0;
const float reflectivity = 0.6;
const float refractivity = 1.33;

void main() {
    vec2 ndc = ClipSpace.xy / ClipSpace.w * 0.5 + 0.5;

    vec2 distortedTexCoords = TexCoords + texture(dudvMap, TexCoords + vec2(moveFactor)).rg * 0.1;
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    vec3 normalMapColor = texture(normalMap, distortedTexCoords).rgb;
    vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0));

    vec3 I = normalize(FragPos - lightPos);
    vec3 R = reflect(I, normalize(normal));

    vec2 refractionOffset = R.xy * 0.05; // 調整偏移強度
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
    vec2 refractionCoord = ndc + refractionOffset;

    reflectTexCoords += totalDistortion;
    reflectTexCoords = clamp(reflectTexCoords, vec2(0.001, -0.999), vec2(0.999, -0.001));

    refractionCoord += totalDistortion;
    refractionCoord = clamp(refractionCoord, vec2(0.001, 0.001), vec2(0.999, 0.999));

    // Fresnel
    vec3 viewDir = normalize(viewPos - FragPos);
    float fresnel = pow(1.0 - max(dot(viewDir, normalize(Normal)), 0.0), 1.5);
    float viewAngle = 1.0 - abs(dot(viewDir, vec3(0.0, 1.0, 0.0)));
    fresnel = mix(fresnel, 1.0, viewAngle * 0.5);
    
    // Light
    float specular = pow(max(dot(R, viewDir), 0.0), shineDamper);
    vec3 specularLight = lightColor * specular * (reflectivity * 0.5);

    // Water depth
    float depth = length(viewPos - FragPos);
    float refractionStrength = clamp(depth * 0.05, 0.0, 1.0);

    vec4 waterColor = vec4(0.0, 0.2, 0.3, 0.1);
    vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
    vec4 refractionColor = texture(refractionTexture, refractionCoord);
    refractionColor = mix(refractionColor, waterColor, refractionStrength);

    vec4 finalColor = mix(refractionColor, reflectionColor, fresnel);
    //finalColor = mix(finalColor, waterColor, 0.15);

    FragColor = finalColor + vec4(specularLight, 0.0);
    FragColor.rgb = clamp(FragColor.rgb, 0.0, 1.0);
}
