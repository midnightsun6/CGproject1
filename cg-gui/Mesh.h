#pragma once

#include "Shader.h"
#include "Animator.h"

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
	std::vector<Mesh> children;

	GLuint VAO, VBO, EBO;
	glm::mat4 transform, invTransform;

	unsigned int getWhiteTexture();
	void setupMesh();

public:
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material);
	Mesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material, glm::vec3 center);

	void setCenter(glm::vec3 center);
	void addChild(Mesh mesh);
	void draw(const Shader& shader, Animator& animator, const glm::mat4& parentModel);

	void setName(const std::string& name);
	void setTransform(const glm::mat4 transform);

	std::string getName() const;
	const std::vector<Mesh>& getChildren() const;
};

