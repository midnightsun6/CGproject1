#pragma once

#include "Mesh.h"

class Model {
private:
	std::vector<Texture> texturesLoaded;
	std::vector<Mesh> meshes;
	std::string directory;
	Animator animator;

	glm::vec3 center;
	glm::mat4 transform, invTransform;
	glm::mat4 modelMatrix;

	std::unordered_map<std::string, Mesh> meshTable; // <name, index> to create child-parent meshes.

	void loadModel(std::string filename);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	void resortMesh();

public:
	/* Initial */
	Model();
	Model(std::string& path);

	void setModel(const char* filename);

	/* Model Animation */
	void importAnimation(const char* filename);
	void exportAnimtion(const char* filename, const char* animation);
	void addAnimation(AnimationClip clip);
	void deleteAnimation(const char* animation);
	void playAnimation(const char* name);

	/* Model Operation */
	void resetMatrix();
	void transformate(float tra[3], float rot[3], float sca[3]);

	/* Function */
	void update(float dt);
	void draw(Shader& shader);

	/* Getter */
	const std::vector<Mesh>& getMeshes() const;
	const Animator& getAnimator() const;
};