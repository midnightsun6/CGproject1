#include "GLBaseObject.h"

GLBaseObject::GLBaseObject() {}

void GLBaseObject::Init(GLenum type) {
	this->type = type;
	model = glm::mat4(1.f);

	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// VBO
	glGenBuffers(1, &pVBOs);
	glBindBuffer(GL_ARRAY_BUFFER, pVBOs);

	glGenBuffers(1, &cVBOs);
	glBindBuffer(GL_ARRAY_BUFFER, cVBOs);

	// EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

void GLBaseObject::setShader(const char* vert, const char* frag) {
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, vert },
		{ GL_FRAGMENT_SHADER, frag },
		{ GL_NONE, NULL }
	};
	shaderProgram = LoadShaders(shaders);
}

void GLBaseObject::addPoint(glm::vec3 point) {
	points.push_back(point);
}

void GLBaseObject::addColor(glm::vec3 color) {
	colors.push_back(color);
}

void GLBaseObject::addIndex(unsigned int index) {
	indices.push_back(index);
}

void GLBaseObject::setModel(glm::mat4 model) {
	this->model = model;
}

void GLBaseObject::Translate(float x, float y, float z) {
	model = glm::translate(glm::mat4(1.f), glm::vec3(x, y, z)) * model;
}

void GLBaseObject::Rotate(float angle, float x, float y, float z) {
	model = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(x, y, z)) * model;
}

void GLBaseObject::Scale(float x, float y, float z) {
	model = glm::scale(glm::mat4(1.f), glm::vec3(x, y, z)) * model;
}

void GLBaseObject::Render(glm::mat4 projection, glm::mat4 view) {
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projectionMatrix"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "modelMatrix"), 1, GL_FALSE, &model[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, pVBOs);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, cVBOs);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

	glDrawElements(type, indices.size(), GL_UNSIGNED_INT, 0);
}