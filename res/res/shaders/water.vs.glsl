#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 ClipSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform sampler2D heightMap;
uniform float waveHeight;
uniform bool isHeightMap;

const float tiling = 6.0;
const float PI = 3.14159265359;

// Wave parameters
const int WAVE_COUNT = 4;
const float WAVE_LENGTH[WAVE_COUNT] = float[](30.0, 25.0, 15.0, 8.0);  // 增加波長
const float STEEPNESS[WAVE_COUNT] = float[](0.3, 0.4, 0.1, 0.2);      // 增加陡度
const vec2 WAVE_DIR[WAVE_COUNT] = vec2[](
    vec2(1.0, 0.6),
    vec2(-0.7, 0.3),
    vec2(0.3, 0.7), 
    vec2(-0.5, 0.8)
);

vec3 gerstnerWave(vec3 position, float amplitude, float frequency, vec2 direction, float phase) {
    direction = normalize(direction);
    float k = 2.0 * 3.14159 / frequency;
    float f = k * (dot(direction, position.xz) - phase);
    float steepness = amplitude / frequency;
    
    return vec3(
        direction.x * (steepness * cos(f)),
        amplitude * sin(f),
        direction.y * (steepness * cos(f))
    );
}

void main()
{
    if (isHeightMap) {
        // 使用時間偏移 UV 座標來製造動態效果
        vec2 uv = aTexCoords;
        uv.x += time * 0.1; // X 方向移動
        uv.y += time * 0.05; // Y 方向移動
        
        // 從 height map 取得高度值
        float height = texture(heightMap, uv).r - 0.5;
        
        // 位移頂點位置
        vec3 pos = aPos;
        pos.y += height * waveHeight;
        
        // 計算法線
        vec2 texelSize = 1.0 / textureSize(heightMap, 0);
        float heightL = texture(heightMap, uv - vec2(texelSize.x, 0.0)).r;
        float heightR = texture(heightMap, uv + vec2(texelSize.x, 0.0)).r;
        float heightD = texture(heightMap, uv - vec2(0.0, texelSize.y)).r;
        float heightU = texture(heightMap, uv + vec2(0.0, texelSize.y)).r;
        
        vec3 normal = normalize(vec3(
            heightL - heightR,
            2.0,
            heightD - heightU
        ));

        FragPos = vec3(model * vec4(pos, 1.0));
        Normal = normal;
        TexCoords = aTexCoords;
        
        ClipSpace = projection * view * vec4(FragPos, 1.0);
        gl_Position = ClipSpace;
    } else {
        vec3 pos = aPos;

        vec3 offset = vec3(0.0);
        for(int i = 0; i < WAVE_COUNT; i++) {
            float speed = 5.0 + float(i) * 0.5;
            float amplitude = STEEPNESS[i] * 0.5;
            offset += gerstnerWave(pos, amplitude, WAVE_LENGTH[i], WAVE_DIR[i], time * speed);
        }
        pos += offset;

        FragPos = vec3(model * vec4(pos, 1.0));
        
        vec3 tangent = vec3(1.0, 0.0, 0.0);
        vec3 bitangent = vec3(0.0, 0.0, 1.0);
        for(int i = 0; i < WAVE_COUNT; i++) {
            float k = 2.0 * PI / WAVE_LENGTH[i];
            float a = STEEPNESS[i];
            vec2 dir = normalize(WAVE_DIR[i]);
            float f = k * (dot(dir, pos.xz) - time);
            
            tangent.y += dir.x * k * a * cos(f);
            bitangent.y += dir.y * k * a * cos(f);
        }
        Normal = normalize(cross(bitangent, tangent));

        ClipSpace = projection * view * vec4(FragPos, 1.0);
        gl_Position = ClipSpace;

        TexCoords = vec2(aTexCoords.x / 2.0 + 0.5, aTexCoords.y / 2.0 + 0.5) * tiling;
    }
}
