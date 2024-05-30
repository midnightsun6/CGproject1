#pragma once
#include "Definition.h"
#include "Shader.h"

class Water {
private:
    const float WAVE_SPEED = 0.03;
    float moveFactor;

    int screenWidth, screenHeight;

    GLuint VAO, VBO, insVBO, EBO;
    GLuint reflectionFBO, reflectionTexture, depthRBO;
    std::vector<float> vertices;
    std::vector<uint> indices;
    std::vector <glm::vec3> offsets;

    std::vector<GLuint> textureID;

    void loadTexture();
    void loadVertices();
    void setupReflection();
    void setupMesh();

public:
    Water();

    void setWindowSize(int width, int height);
    void init();
    void update(float dt);
    void draw(const Shader& shader, GLuint skyboxTexture);

    const GLuint& getReflectionFBO();
};

