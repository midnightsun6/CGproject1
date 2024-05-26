#pragma once
#include "Definition.h"
#include "Shader.h"

class MotionBlur {
private:
    int screenWidth, screenHeight;
    float blurScale;

    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    GLuint velocityFBO, colorFBO;
    GLuint velocityTexture, colorTexture;

public:
    MotionBlur();

    void setScreenSize(int width, int height);
    void setBlurScale(float blurScale);

    const GLuint& getVelocityFBO() const;
    const GLuint& getVelocityTexture() const;
    const GLuint& getColorFBO() const;
    const GLuint& getColorTexture() const;

    void init();
    void renderMotionBlur(const Shader& shader);
};

