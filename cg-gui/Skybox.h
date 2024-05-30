#pragma once
#include "Definition.h"
#include "Shader.h"

class Skybox {
private:
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::string> faces;
	GLuint VAO, VBO, EBO;
	GLuint cubemap;

	void loadTexture();

public:
	Skybox();

	const GLuint& getSkyboxTexture() const;

	void drawPrevVelocity(const Shader& shader);
	void draw(const Shader& shader);
};

