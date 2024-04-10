#pragma once
#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	std::vector<int> bonesID[MAX_BONE_INFLUENCE];
	std::vector<float> weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	GLuint id;
	std::string type;
	aiString path;
};

struct Material {
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
	float shininess;
};

class Mesh {
private:
	std::string name;
	Material material;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	GLuint VAO, VBO, EBO;

	glm::mat4 transform = glm::mat4(1.0f);

	unsigned int getWhiteTexture();
	void setupMesh();

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material);
	Mesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material);

	void draw(const Shader& shader);
};

