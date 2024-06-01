#include "PointLightCube.h"

void PointLightCube::setupDepthCubemap() {
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthCubemap);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLightCube::setupMesh() {
    // Set points position.
    float size = 0.5;
    vertices.push_back(glm::vec3(-size, -size, -size));   // 0
    vertices.push_back(glm::vec3(-size, -size, size));    // 1
    vertices.push_back(glm::vec3(-size, size, -size));    // 2
    vertices.push_back(glm::vec3(-size, size, size));     // 3
    vertices.push_back(glm::vec3(size, -size, -size));    // 4
    vertices.push_back(glm::vec3(size, -size, size));     // 5
    vertices.push_back(glm::vec3(size, size, -size));     // 6
    vertices.push_back(glm::vec3(size, size, size));      // 7

    indices.push_back(1); indices.push_back(5); indices.push_back(7);
    indices.push_back(7); indices.push_back(3); indices.push_back(1);

    indices.push_back(5); indices.push_back(4); indices.push_back(6);
    indices.push_back(6); indices.push_back(7); indices.push_back(5);

    indices.push_back(4); indices.push_back(0); indices.push_back(2);
    indices.push_back(2); indices.push_back(6); indices.push_back(4);

    indices.push_back(0); indices.push_back(1); indices.push_back(3);
    indices.push_back(3); indices.push_back(2); indices.push_back(0);

    indices.push_back(3); indices.push_back(7); indices.push_back(6);
    indices.push_back(6); indices.push_back(2); indices.push_back(3);

    indices.push_back(4); indices.push_back(5); indices.push_back(1);
    indices.push_back(1); indices.push_back(0); indices.push_back(4);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

PointLightCube::PointLightCube() {
    this->SHADOW_WIDTH = this->SHADOW_HEIGHT = 2048;
    this->position = glm::vec3(0.f);
    this->color = glm::vec3(1.f);
    this->nearPlane = 0.1f;
    this->farPlane = 150.f;
    this->projection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

    this->setupDepthCubemap();
    this->setupMesh();
}

void PointLightCube::setVision(float nearPlane, float farPlane) {
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
    this->projection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
}

void PointLightCube::setPosition(const glm::vec3& pos) {
    this->position = pos;
}

void PointLightCube::setColor(const glm::vec3& color) {
    this->color = color;
}

const uint& PointLightCube::getShadowWidth() const {
    return this->SHADOW_WIDTH;
}

const uint& PointLightCube::getShadowHeight() const {
    return this->SHADOW_HEIGHT;
}

const float& PointLightCube::getFarPlane() const {
    return this->farPlane;
}

const glm::vec3& PointLightCube::getPosition() const {
    return this->position;
}

const glm::vec3& PointLightCube::getColor() const {
    return this->color;
}

std::vector<glm::mat4> PointLightCube::getViewMatrices() const {
    std::vector<glm::mat4> views;
    views.push_back(projection * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    views.push_back(projection * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    views.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    views.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    views.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    views.push_back(projection * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    return views;
}

const GLuint& PointLightCube::getDepthMapFBO() const {
    return this->depthMapFBO;
}

const GLuint& PointLightCube::getDepthCubemap() const {
    return this->depthCubemap;
}

void PointLightCube::draw(const Shader& shader) {
    glm::mat4 model = glm::translate(glm::mat4(1.f), position);

    shader.setUniform("model", model);
    shader.setUniform("lightColor", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
