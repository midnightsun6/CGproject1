#include "Terrian.h"

void Terrian::loadTexture() {
	int width, height, nrComponents;
	unsigned char* data = stbi_load("./res/terrian/terrian.jpg", &width, &height, &nrComponents, 0);
	if (data) {
        GLenum format;
        if (nrComponents == 1) {
            format = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
        }
        else if (nrComponents == 4) {
            format = GL_RGBA;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Texture failed to load at path: ./res/Terrian/terrian.jpg\n";
	}
	stbi_image_free(data);
}

Terrian::Terrian() {
    float size = 0.5f;
    vertices = {
        size,  size, 0.0f,  1.0f, 1.0f,   // ¥k¤W¨¤
        size, -size, 0.0f,  1.0f, 0.0f,   // ¥k¤U¨¤
       -size, -size, 0.0f,  0.0f, 0.0f,   // ¥ª¤U¨¤
       -size,  size, 0.0f,  0.0f, 1.0f    // ¥ª¤W¨¤
    };

    /*
        3¢w¢w¢w0
        |   |
        |   |
        2¢w¢w¢w1
    */

    indices = {
        0, 1, 2,
        3, 0, 2
    };

    float offset = 1.0f;
    int boundary = 2000;
    for (int i = -boundary; i < boundary; i += 10) {
        for (int j = -boundary; j < boundary; j += 10) {
            glm::vec3 trans;
            trans.x = (float)i / 10.f + offset;
            trans.y = (float)j / 10.f + offset;
            trans.z = 0.f;
            offsets.push_back(trans);
        }
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glGenBuffers(1, &insVBO);
    glBindBuffer(GL_ARRAY_BUFFER, insVBO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glActiveTexture(GL_TEXTURE4);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    this->loadTexture();
}

void Terrian::drawPrevVelocity(const Shader& shader) {
    shader.setUniform("model", glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1, 0, 0)));
    shader.setUniform("prevModel", glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1, 0, 0)));
    shader.setUniform("isInstanced", true);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, insVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, insVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, offsets.size());

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Terrian::draw(const Shader& shader) {
    shader.setUniform("model", glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1, 0, 0)));

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setUniform("texture1", 4);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, insVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, offsets.size());
}
