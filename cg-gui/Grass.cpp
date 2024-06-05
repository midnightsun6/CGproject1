#include "Grass.h"

Grass::Grass() {
    vertices = {
        0.5f, 1.0f, 0.0f ,
        0.5f, 0.0f, 0.0f ,
        -0.5f, 0.0f, 0.0f ,
        -0.5f, 1.0f, 0.0f ,

        0.0f , 1.0f, 0.5f,
        0.0f , 0.0f, 0.5f,
        0.0f , 0.0f,-0.5f,
        0.0f , 1.0f,-0.5f,
    };

    textures = {
        1.0f, 1.0f ,
        1.0f, 0.0f ,
        0.0f, 0.0f ,
        0.0f, 1.0f ,

        1.0f, 1.0f ,
        1.0f, 0.0f ,
        0.0f, 0.0f ,
        0.0f, 1.0f
    };
    
    indices = { 
        0, 1, 3,   
        1, 2, 3,

        4, 5, 7,
        5, 6, 7
    };

    loadTexture();

    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &velocityVAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBOTextures);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTextures);
    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), &textures[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    offsets.resize(grassNum);

    srand(time(NULL));

    for (int i = 0; i < grassNum; ++i) {
        float x = static_cast<float>(rand() % grassRangeX) - grassRangeX / 2; 
        float z = static_cast<float>(rand() % grassRangeZ) - grassRangeZ / 2;  

        offsets[i] = glm::vec3(x, 0, z);
    }

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);
}

void Grass::loadTexture()
{
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(3, grassTextures);

    for (int i = 0; i < 3; i++) {
        int width, height, nrComponents;
        unsigned char* data = stbi_load(grass[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glActiveTexture(GL_TEXTURE15 + i);
            glBindTexture(GL_TEXTURE_2D, grassTextures[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << grass[i] << std::endl;
            stbi_image_free(data);
        }
    }
    stbi_set_flip_vertically_on_load(false);
}

void Grass::drawPrevVelocity(const Shader& shader) {
    shader.setUniform("model", glm::mat4(1.f));
    shader.setUniform("prevModel", glm::mat4(1.f));
    shader.setUniform("isInstanced", true);

    glBindVertexArray(velocityVAO);

    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE15 + i);
        glBindTexture(GL_TEXTURE_2D, grassTextures[i]);
    }
    shader.setUniform("texture1", 15);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0, grassNum);

    glBindVertexArray(0);
}

void Grass::draw(const Shader& shader) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.setUniform("model", glm::mat4(1.0f));
   
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBOTextures);
    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), &textures[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec3), &offsets[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    for (int i = 0; i < 3; ++i) {
        glActiveTexture(GL_TEXTURE15 + i);
        glBindTexture(GL_TEXTURE_2D, grassTextures[i]);
    }
    shader.setUniform("texture1", 15);
    
    glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0, grassNum);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}
