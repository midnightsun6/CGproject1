#pragma once
#include "Definition.h"
#include "Shader.h"

class Grass {
private:

	std::vector<float> vertices;
	std::vector<float> textures;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> instancePositions;
	GLuint grassTextures[3];
	std::string grass[3] = { "res/terrian/Grass.png" , "res/terrian/Large_Fern.png", "res/terrian/Tall_Grass.png" };
	GLuint VAO, VBO, VBOTextures, instanceVBO, EBO;
	GLuint texture;
	int grassNum = 10000;
	int grassRangeX = 399, grassRangeZ = 399;

	void loadTexture();	    
public:
	Grass();
	void draw(const Shader& shader);
};