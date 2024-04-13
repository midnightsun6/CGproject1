#pragma once

#include "Mesh.h"

class Model {
private:
	std::vector<Texture> texturesLoaded;
	std::vector<Mesh> meshes;
	std::string directory;

	Animator animator;
	std::unordered_map<std::string, Mesh> meshTable; // <name, index> to create child-parent meshes.
	std::vector<std::string> parts;

	void loadModel(std::string filename);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	void resortMesh();

public:
	Model();
	Model(std::string& path);

	void setModel(const char* filename);
	void addAnimation(const char* filename);
	void playAnimation(const char* name);
	void update(float dt);
	void draw(Shader& shader);

	const std::vector<std::string>& getPartsName();
};