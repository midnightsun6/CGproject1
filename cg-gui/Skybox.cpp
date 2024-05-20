#include "Skybox.h"

void Skybox::loadTexture() {
    std::string path = "./res/background/";
    int width, height, nrChannels;
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load((path + faces[i]).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << '\n';
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Skybox::Skybox() {
    // Set points position.
    float size = 200.f;
    vertices.push_back(glm::vec3(-size, -size, -size));   // 0
    vertices.push_back(glm::vec3(-size, -size, size));    // 1
    vertices.push_back(glm::vec3(-size, size, -size));    // 2
    vertices.push_back(glm::vec3(-size, size, size));     // 3
    vertices.push_back(glm::vec3(size, -size, -size));    // 4
    vertices.push_back(glm::vec3(size, -size, size));     // 5
    vertices.push_back(glm::vec3(size, size, -size));     // 6
    vertices.push_back(glm::vec3(size, size, size));      // 7

    // Set indices.
    /*
            6-------7
           /|      /|
          2-------3 |
          | |     | |
          | 4-----|-5
          |/      |/
          0-------1
    */
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

    faces = {
        "px.jpg", "nx.jpg", "py.jpg", "ny.jpg", "pz.jpg", "nz.jpg"
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    this->loadTexture();
}

void Skybox::draw(const Shader& shader) {
    glDepthMask(GL_FALSE);

    shader.setUniform("skybox", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
}
