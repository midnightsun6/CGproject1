#pragma once

#include "Mesh.h"

class Model {
private:
	std::vector<Texture> texturesLoaded;
	std::vector<Mesh> meshes;
	std::string directory;

	std::unordered_map<std::string, Mesh> meshTable; // <name, index> to create child-parent meshes.

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	void ResortMesh();

public:
	Model();
	Model(const char* path);

	void setModel(const char* path);
	void draw(Shader& shader);
};