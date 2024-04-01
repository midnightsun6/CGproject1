#pragma once
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	std::string type;
	aiString path;
};

class Mesh {
private:
	GLuint VAO, VBO, EBO;

	void setupMesh();

public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	void draw(Shader& shader);
};

