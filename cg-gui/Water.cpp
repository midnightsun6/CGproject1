#include "Water.h"

void Water::loadTexture() {
    textureID.resize(2);
    std::vector<std::string> paths = { 
        "./res/background/water_normal_map2.jpg", 
        "./res/background/water_dudv_map.jpg" 
    };

    for (int i = 0; i < textureID.size(); i++) {
        GLuint& texture = textureID[i];
        std::string& path = paths[i];

        glGenTextures(1, &texture);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glActiveTexture(GL_TEXTURE12 + i);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else {
            std::cout << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
    }
}

void Water::loadVertices() {
    float size = 200.0f;
    vertices = {
        // positions         // normals         // texCoords
        -size,  0.0f, -size,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         size,  0.0f, -size,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         size,  0.0f,  size,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -size,  0.0f,  size,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    };

    indices = {
        0, 1, 2,
        2, 3, 0
    };

    float offset = 1.0f;
    int boundary = 2000;
    /*for (int i = -boundary; i < boundary; i += 10) {
        for (int j = -boundary; j < boundary; j += 10) {
            glm::vec3 trans;
            trans.x = (float)i / 10.f + offset;
            trans.y = 0.f;
            trans.z = (float)j / 10.f + offset;
            offsets.push_back(trans);
        }
    }*/
    offsets.push_back(glm::vec3(0.f));
}

void Water::setupReflection() {
    glGenFramebuffers(1, &reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    
    glGenTextures(1, &reflectionTexture);
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);

    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Reflection Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &insVBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, insVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

Water::Water() {
    this->moveFactor = 0;
}

void Water::setWindowSize(int width, int height) {
    this->screenWidth = width;
    this->screenHeight = height;
    this->init();
}

void Water::init() {
    this->loadTexture();
    this->loadVertices();
    this->setupMesh();
    this->setupReflection();
}

void Water::update(float dt) {
    this->moveFactor += WAVE_SPEED * dt;
    this->moveFactor = std::fmod(moveFactor, 1.f);
}

void Water::draw(const Shader& shader, GLuint skyboxTexture) {
    shader.setUniform("model", glm::mat4(1.f));
    shader.setUniform("moveFactor", moveFactor);

    // skybox texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
    shader.setUniform("skybox", 3);

    // Water normal map
    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    shader.setUniform("normalMap", 12);

    // Water dudv map
    glActiveTexture(GL_TEXTURE13);
    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    shader.setUniform("dudvMap", 13);

    // Water reflection
    glActiveTexture(GL_TEXTURE14);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    shader.setUniform("reflectionTexture", 14);

    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, offsets.size());
    glBindVertexArray(0);
}

const GLuint& Water::getReflectionFBO() {
    return this->reflectionFBO;
}
