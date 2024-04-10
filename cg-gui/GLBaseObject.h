#pragma once

#include "Shader.h"

class GLBaseObject {
protected:
	GLenum type;

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> colors;
	std::vector<unsigned int> indices;
	GLuint shaderProgram;
	GLuint VAO;
	GLuint pVBOs;	// a point of buffer.
	GLuint cVBOs;	// a color of buffer.
	GLuint EBO;

	glm::mat4 model;

public:
	GLBaseObject();

	virtual void Init(GLenum type);

	void addPoint(glm::vec3 point);
	void addColor(glm::vec3 color);
	void addIndex(unsigned int index);

	void setModel(glm::mat4 model);
	void Translate(float x, float y, float z);
	void Rotate(float angle, float x, float y, float z);
	void Scale(float x, float y, float z);

	virtual void Draw(glm::mat4 projection, glm::mat4 view, Shader& shader);
};

