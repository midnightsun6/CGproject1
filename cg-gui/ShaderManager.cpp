#include "ShaderManager.h"

ShaderManager ShaderManager::shaderManager;

ShaderManager::ShaderManager() {}

ShaderManager* ShaderManager::getInstance() {
    return &shaderManager;
}

void ShaderManager::init() {
    this->shaders = {
        Shader("shader.vs.glsl", "shader.fs.glsl"),
        Shader("model_loading.vs.glsl", "model_loading.fs.glsl"),
        Shader("model_loading.vs.glsl", "model_loading_toon.fs.glsl"),
        Shader("cubemap.vs.glsl", "cubemap.fs.glsl"),
        Shader("terrian.vs.glsl", "terrian.fs.glsl"),
        Shader("grass.vs.glsl", "grass.fs.glsl"),
        Shader("particle.vs.glsl", "particle.gs.glsl", "particle.fs.glsl"),
        Shader("depth_map.vs.glsl", "depth_map.fs.glsl"),
        Shader("reflection.vs.glsl", "reflection.fs.glsl"),
        Shader("velocity.vs.glsl", "velocity.fs.glsl"),
        Shader("velocity_particle.vs.glsl", "velocity_particle.gs.glsl", "velocity_particle.fs.glsl"),
        Shader("motion_blur.vs.glsl", "motion_blur.fs.glsl"),
        Shader("mosaic.vs.glsl", "mosaic.fs.glsl"),
        Shader("water.vs.glsl", "water.fs.glsl"),
    };
}

void ShaderManager::useShader(Shaders shader) {
    this->currShader = shader;
    this->shaders[currShader].use();
}

const Shader& ShaderManager::getCurrShader() {
    return this->shaders[currShader];
}
