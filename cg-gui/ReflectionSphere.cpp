#include "ReflectionSphere.h"

ReflectionSphere::ReflectionSphere() {}

void ReflectionSphere::init(float radius, int slices, int stacks) {
    this->model = prevModel = glm::mat4(1.f);

    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &velocityVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    for (int i = 0; i <= stacks; i++) {
        float phi = i / (float)stacks * glm::pi<float>();

        for (int j = 0; j <= slices; j++) {
            float theta = j / (float)slices * (glm::pi<float>() * 2);

            float x = cos(theta) * sin(phi) * radius;
            float y = cos(phi) * radius;
            float z = sin(theta) * sin(phi) * radius;

            glm::vec3 position = glm::vec3(x, y, z);
            glm::vec3 normal = glm::normalize(position);
            glm::vec2 texCoords = glm::vec2(j / (float)slices, i / (float)stacks);

            data.push_back(position.x);
            data.push_back(position.y);
            data.push_back(position.z);
            data.push_back(normal.x);
            data.push_back(normal.y);
            data.push_back(normal.z);
            data.push_back(texCoords.x);
            data.push_back(texCoords.y);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glGenTextures(1, &envCubemap);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, captureSize, captureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 1000.0f);
    captureViews.resize(6);
    glm::vec3 viewPos = glm::vec3(0.f, 30.f, 0.f);
    captureViews[0] = glm::lookAt(viewPos, viewPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    captureViews[1] = glm::lookAt(viewPos, viewPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    captureViews[2] = glm::lookAt(viewPos, viewPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    captureViews[3] = glm::lookAt(viewPos, viewPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    captureViews[4] = glm::lookAt(viewPos, viewPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    captureViews[5] = glm::lookAt(viewPos, viewPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
}

void ReflectionSphere::translate(float dx, float dy, float dz) {
    this->model = glm::translate(glm::mat4(1.f), glm::vec3(dx, dy, dz)) * model;
}

void ReflectionSphere::drawPrevVelocity(const Shader& shader) {
    shader.setUniform("model", model);
    shader.setUniform("prevModel", prevModel);
    shader.setUniform("isInstanced", false);

    glBindVertexArray(velocityVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    unsigned int stride = (3 + 3 + 2) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ReflectionSphere::draw(const Shader& shader) {
    shader.setUniform("model", model);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    shader.setUniform("skybox", 5);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    unsigned int stride = (3 + 3 + 2) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    prevModel = model;
}

const int& ReflectionSphere::getCaptureSize() {
    return this->captureSize;
}

const GLuint& ReflectionSphere::getCaptureFBO() {
    return this->captureFBO;
}

const GLuint& ReflectionSphere::getCaptureRBO() {
    return this->captureRBO;
}

const GLuint& ReflectionSphere::getEnvCubemap() {
    return this->envCubemap;
}

const glm::mat4& ReflectionSphere::getCaptureProjection() {
    return this->captureProjection;
}

const glm::mat4& ReflectionSphere::getCaptureView(int idx) {
    if (0 <= idx && idx < captureViews.size()) {
        return this->captureViews[idx];
    }
    return glm::mat4(1.f);
}
