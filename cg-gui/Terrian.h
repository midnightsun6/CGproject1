#pragma once
#include "Definition.h"
#include "Shader.h"

class Terrian {
private:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> translation;
	GLuint texture;
	GLuint VAO, VBO, EBO;
	GLuint insVBO;

	void loadTexture();

public:
	Terrian();

	void draw(const Shader& shader);
};

