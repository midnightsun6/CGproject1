#pragma once
#include "Shader.h"

class ReflectionSphere {
private:
    GLuint VAO, VBO, EBO;
    GLuint captureFBO, captureRBO, envCubemap;
    int captureSize = 1024;

    glm::mat4 model, prevModel;
    glm::mat4 captureProjection;
    std::vector<glm::mat4> captureViews;
    std::vector<float> data;
    std::vector<unsigned int> indices;

public:
    ReflectionSphere();
    void init(float radius, int slices, int stacks);

    void translate(float dx, float dy, float dz);

    void drawPrevVelocity(const Shader& shader);
    void draw(const Shader& shader);

    const int& getCaptureSize();
    const GLuint& getCaptureFBO();
    const GLuint& getCaptureRBO();
    const GLuint& getEnvCubemap();
    const glm::mat4& getCaptureProjection();
    const glm::mat4& getCaptureView(int idx);
};

