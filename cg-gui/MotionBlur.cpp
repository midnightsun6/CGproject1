#include "MotionBlur.h"

MotionBlur::MotionBlur() {}

void MotionBlur::setScreenSize(int width, int height) {
    this->screenWidth = width;
    this->screenHeight = height;
    this->init();
}

void MotionBlur::setBlurScale(float blurScale) {
    this->blurScale = blurScale;
}

void MotionBlur::init() {
    blurScale = 3.0f;

    // Create velocity texture
    glActiveTexture(GL_TEXTURE8);
    glGenTextures(1, &velocityTexture);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, screenWidth, screenHeight, 0, GL_RG, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create color texture
    glActiveTexture(GL_TEXTURE7); 
    glGenTextures(1, &colorTexture);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Create velocity framebuffer
    glGenFramebuffers(1, &velocityFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, velocityFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, velocityTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create color framebuffer
    glGenFramebuffers(1, &colorFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    vertices = {
        // positions        // texCoords
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f
    };

    indices = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

const GLuint& MotionBlur::getVelocityFBO() const {
    return this->velocityFBO;
}

const GLuint& MotionBlur::getVelocityTexture() const {
    return this->velocityTexture;
}

const GLuint& MotionBlur::getColorFBO() const {
    return this->colorFBO;
}

const GLuint& MotionBlur::getColorTexture() const {
    return this->colorTexture;
}

void MotionBlur::renderMotionBlur(const Shader& shader) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    shader.setUniform("colorTexture", 7);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, velocityTexture);
    shader.setUniform("velocityTexture", 8);

    shader.setUniform("blurScale", blurScale);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
