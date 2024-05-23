#pragma once
#include "Shader.h"

enum Shaders {
    SHADER_BASE_SHAPE,
    SHADER_MODEL,
    SHADER_MODEL_TOON,
    SHADER_MODEL_MOSAIC,
    SHADER_CUBEMAP,
    SHADER_TERRIAN,
    SHADER_GRASS,
    SHADER_PARTICLE_FOUNTAIN,
    SHADER_DEPTH_MAP,
    SHADER_REFLECTION,
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
