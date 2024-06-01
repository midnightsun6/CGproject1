#pragma once
#include "Shader.h"

enum Shaders {
    SHADER_BASE_SHAPE,
    SHADER_MODEL,
    SHADER_MODEL_TOON,
    SHADER_CUBEMAP,
    SHADER_TERRIAN,
    SHADER_GRASS,
    SHADER_PARTICLE_FOUNTAIN,
    SHADER_DEPTH_CUBEMAP,
    SHADER_REFLECTION,
    SHADER_VELOCITY,
    SHADER_VELOCITY_PARTICLE,
    SHADER_MONTION_BULR,
    SHADER_MOSAIC,
    SHADER_WATER,
    SHADER_LIGHT_CUBE,
    SHADER_NONE,
};

class ShaderManager {
private:
    std::vector<Shader> shaders;
    Shaders currShader;

    // Singleton system design parttern
    static ShaderManager shaderManager;
    ShaderManager();

public:
    static ShaderManager* getInstance();
    void init();

    void useShader(Shaders shader);

    template<class T>
    inline void setCurrUniform(std::string name, const T& value);
    const Shader& getCurrShader();
};

template<class T>
inline void ShaderManager::setCurrUniform(std::string name, const T& value) {
    this->shaders[currShader].setUniform(name, value);
}
