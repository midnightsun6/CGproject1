#include "Mesh.h"

unsigned int Mesh::getWhiteTexture() {
	static unsigned int id = 0;
	if (id == 0) {
		std::vector<unsigned char> whiteData = { 255, 255, 255, 255 };
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whiteData[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	return id;
}

void Mesh::setupMesh() {
	this->SHADOW_HEIGHT = this->SHADOW_WIDTH = 1024;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &insVBO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Set vertex pointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Set normal pointer
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// Set texture pointer
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoords));

	// Set tangent pointer
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));

	// Set bitangent pointer
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));

	// Set Bones ID pointer
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bonesID));

	// Set weights pointer
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, weights));

	glBindVertexArray(0);

	// Add depth map (for shadow)
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = material;

	setupMesh();
}

Mesh::Mesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material material, glm::vec3 center) {
	this->name = name;
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = material;
	this->transform = glm::translate(glm::mat4(1.f), center);
	this->invTransform = glm::translate(glm::mat4(1.f), -center);

	setupMesh();
}

std::string Mesh::getName() const {
	return this->name;
}

const std::vector<Mesh>& Mesh::getChildren() const {
	return this->children;
}

void Mesh::setCenter(glm::vec3 center) {
	this->transform = glm::translate(glm::mat4(1.f), center);
	this->invTransform = glm::translate(glm::mat4(1.f), -center);
}

void Mesh::addChild(Mesh mesh) {
	children.push_back(mesh);
}

void Mesh::drawDepthMap(const Shader& shader, Animator& animator, const glm::mat4& parentModel, const std::vector<glm::vec2>& offsets, const int& amount) {
	glm::mat4 animation = animator.getAnimationMatrix(this->name);
	glm::mat4 model = parentModel * transform * animation * invTransform;
	shader.setUniform("model", model);

	for (auto& child : children) {
		child.drawDepthMap(shader, animator, model, offsets, amount);
	}

	// Draw a mesh.
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindVertexArray(VAO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	glBindBuffer(GL_ARRAY_BUFFER, insVBO);
	glBufferData(GL_ARRAY_BUFFER, std::min(amount, (int)offsets.size()) * sizeof(glm::vec2), &offsets[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, std::min(amount, (int)offsets.size()));
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Mesh::draw(const Shader& shader, Animator& animator, const glm::mat4& parentModel, const std::vector<glm::vec2>& offsets, const int& amount) {
	glm::mat4 animation = animator.getAnimationMatrix(this->name);
	glm::mat4 model = parentModel * transform * animation * invTransform;
	//glm::mat4 model = parentModel * transform * animation;
	// Draw the children meshes.
	for (auto& child : children) {
		child.draw(shader, animator, model, offsets, amount);
	}

	shader.setUniform("model", model);
	shader.setUniform("material.diffuse", material.diffuse);
	shader.setUniform("material.specular", material.specular);
	shader.setUniform("material.ambient", material.ambient);
	shader.setUniform("material.shininess", material.shininess);
	shader.setUniform("hasTexture", textures.empty() ? 0 : 1);

	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;
	GLuint heightNr = 1;
	for (int i = 0; i < textures.size(); i++) {
		// Active the texture before bind it.
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}
		else if (name == "texture_normal") {
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_height") {
			number = std::to_string(heightNr++);
		}

		shader.setUniform(name + number, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.setUniform("shadowMap", 1);

	// Draw a mesh.
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, insVBO);
	glBufferData(GL_ARRAY_BUFFER, std::min(amount, (int)offsets.size()) * sizeof(glm::vec2), &offsets[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, std::min(amount, (int)offsets.size()));
	glBindVertexArray(0);
}

void Mesh::setName(const std::string& name) {
	this->name = name;
}

void Mesh::setTransform(const glm::mat4 transform) {
	this->transform = transform;
	this->invTransform = glm::inverse(transform);
}
