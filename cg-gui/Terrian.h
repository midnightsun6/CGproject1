#pragma once
#include "Definition.h"
#include "Shader.h"

class Terrian {
private:
	std::vector<float> vertices, velocityVertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> offsets;
	GLuint texture;
	GLuint VAO, velocityVAO, VBO, EBO;
	GLuint insVBO;

	void loadTexture();

public:
	Terrian();

	void drawPrevVelocity(const Shader& shader);
	void draw(const Shader& shader);
};

