#pragma once

#include "Shader.h"

class PointLightCube {
private:
    uint SHADOW_WIDTH, SHADOW_HEIGHT;
    glm::vec3 position;
    glm::vec3 color;

    float nearPlane, farPlane;
    glm::mat4 projection;

    GLuint VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    std::vector<uint> indices;

    GLuint depthCubemap, depthMapFBO;
    
    void setupDepthCubemap();
    void setupMesh();

public:
    PointLightCube();

    void setVision(float nearPlane, float farPlane);
    void setPosition(const glm::vec3& pos);
    void setColor(const glm::vec3& color);

    const uint& getShadowWidth() const;
    const uint& getShadowHeight() const;
    const float& getFarPlane() const;
    const glm::vec3& getPosition() const;
    const glm::vec3& getColor() const;
    std::vector<glm::mat4> getViewMatrices() const;
    const GLuint& getDepthMapFBO() const;
    const GLuint& getDepthCubemap() const;

    void draw(const Shader& shader);
};

