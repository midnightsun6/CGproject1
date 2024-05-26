#pragma once
#include "Definition.h"
#include "Shader.h"

class Mosaic {
private:
    int screenWidth, screenHeight;
    float blockSize;

    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    GLuint FBO, textureID; 

public:
    Mosaic();

    void setScreenSize(int width, int height);
    void setBlockSize(float blockSize);

    const GLuint& getMosaicFBO() const;
    const GLuint& getMosaicTexture() const;

    void init();
    void renderMosaic(const Shader& shader);
};

