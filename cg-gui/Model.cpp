﻿#include "Model.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

void Model::loadModel(std::string filename) {
	computeInstanceOffsets();

	std::string path = "./res/models/" + filename;

	Assimp::Importer import;
	// aiProcess_Triangulate: If model is not build by triangles, force turn all shape to triangles.
	// aiProcess_FlipUVs: Inverse the texture coordinate at y-axis.
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
		return;
	}

	std::cout << "Loading model\n";
	this->directory = path.substr(0, path.find_last_of('/'));
	this->processNode(scene->mRootNode, scene);
	//this->processNodeFBX(scene->mRootNode, scene, glm::mat4(1.f));
	this->center /= (float)meshes.size();
	this->resortMesh();
	this->transform = glm::translate(glm::mat4(1.f), center);
	this->invTransform = glm::translate(glm::mat4(1.f), -center);
	std::cout << "Loading model Successfully: " << filename << '\n';
}

void Model::computeInstanceOffsets() {
	float offset = 1.0f;
	int boundary = 2000;
	for (int i = 0; i < boundary; i += 100) {
		for (int j = 0; j < boundary; j += 100) {
			glm::vec3 trans;
			trans.x = (float)i / 10.f + offset;
			trans.y = 0.f;
			trans.z = (float)j / 10.f + offset;
			offsets.push_back(trans);
		}
	}
}

void Model::processNodeFBX(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform) {
	// Add all node in meshes.
	std::cout << "HI:\n";
	std::cout << "\tNumMeshes: " << node->mNumMeshes << '\n';
	std::cout << "\tmNumChildren: " << node->mNumChildren << '\n';

	auto convertToMat4 = [](const aiMatrix4x4& from) -> glm::mat4 {
		glm::mat4 to;

		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

		return to;
	};

	std::cout << '\t' << node->mName.C_Str() << '\n';
	aiMatrix4x4 aiTransform = node->mTransformation;
	glm::mat4 transform = parentTransform * convertToMat4(aiTransform);
	for (int i = 0; i < 4; i++) {
		std::cout << "[" << transform[i][0] << ", " << transform[i][1] << ", " << transform[i][2] << ", " << transform[i][3] << "]\n";
	}

	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		std::cout << '\t' << aimesh->mName.C_Str() << '\n';

		// Get mesh
		Mesh mesh = this->processMesh(aimesh, scene);
		mesh.setTransform(transform);
		meshTable[mesh.getName()] = mesh;
		this->meshes.push_back(mesh);

		std::cout << "\t" << mesh.getName() << '\n';
	}

	// Recursion to process all child nodes.
	for (GLuint i = 0; i < node->mNumChildren; i++) {
		this->processNodeFBX(node->mChildren[i], scene, transform);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// Add all node in meshes.
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		
		// Get mesh
		Mesh mesh = this->processMesh(aimesh, scene);
		meshTable[mesh.getName()] = mesh;
		this->meshes.push_back(mesh);
	}

	// Recursion to process all child nodes.
	for (GLuint i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::string name = mesh->mName.C_Str();
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	Material mtl;

	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// Process mesh position.
		glm::vec3 v;
		v.x = mesh->mVertices[i].x;
		v.y = mesh->mVertices[i].y;
		v.z = mesh->mVertices[i].z;
		vertex.position = v;

		// Process mesh normal.
		if (mesh->HasNormals()) {
			v.x = mesh->mNormals[i].x;
			v.y = mesh->mNormals[i].y;
			v.z = mesh->mNormals[i].z;
			vertex.normal = v;
		}
		else {
			vertex.normal = glm::vec3(1.0f);
		}

		// Process mesh texture.
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
			// tangent
			v.x = mesh->mTangents[i].x;
			v.y = mesh->mTangents[i].y;
			v.z = mesh->mTangents[i].z;
			vertex.tangent = v;
			// bitangent
			v.x = mesh->mBitangents[i].x;
			v.y = mesh->mBitangents[i].y;
			v.z = mesh->mBitangents[i].z;
			vertex.bitangent = v;
		}
		else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		if (!diffuseMaps.empty()) {
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}

		std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		if (!textures.empty()) {
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
		if (!heightMaps.empty()) {
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_normal");
		if (!normalMaps.empty()) {
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}


		aiColor3D color(0, 0, 0);
		float shininess;
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			mtl.diffuse = glm::vec3(color.r, color.g, color.b);
		}

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
			mtl.specular = glm::vec3(color.r, color.g, color.b);
		}

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
			mtl.ambient = glm::vec3(color.r, color.g, color.b);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess)) {
			mtl.shininess = shininess;
		}
	}

	// Compute the offset between origin and itself center.
	glm::vec3 sum(0.f, 0.f, 0.f);
	for (GLuint j = 0; j < mesh->mNumVertices; j++) {
		aiVector3D pos = mesh->mVertices[j];
		sum += glm::vec3(pos.x, pos.y, pos.z);
	}
	glm::vec3 center = sum / (float)mesh->mNumVertices;

	this->center += center;
	
	return Mesh(name, vertices, indices, textures, mtl, center);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		GLboolean skip = false;
		for (GLuint j = 0; j < texturesLoaded.size(); j++) {
			if (texturesLoaded[j].path == str) {
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->texturesLoaded.push_back(texture);
		}
	}

	return textures;
}

void Model::resortMesh() {
	for (int i = meshes.size() - 1; ~i; i--) {
		Mesh& mesh = meshTable[meshes[i].getName()];

		// Split the mesh name and parent name
		std::string origin = mesh.getName();
		int selfNameSpace = origin.find_last_of(' ');
		if (selfNameSpace == std::string::npos) {
			continue;
		}
		std::string self = origin.substr(selfNameSpace + 1);
		std::string parent = origin.substr(0, selfNameSpace);
		//std::cout << "Parent: " << parent << ", self: " << self << '\n';
		mesh.setName(self);
		
		// Find parent, if exists, create parent-child hierarchy
		if (meshTable.count(parent)) {
			//std::cout << "Find Parent: " << parent << '\n';
			meshTable[parent].addChild(mesh);
			meshTable.erase(origin);
		}
	}

	meshes.clear();
	for (auto& [_, mesh] : meshTable) {
		meshes.push_back(mesh);
	}

	/*auto dfs = [&](Mesh mesh, auto& dfs) -> void {
		std::cout << mesh.getName() << '\n';
		for (auto& m : mesh.getChildren()) {
			dfs(m, dfs);
		}
	};
	dfs(meshes.front(), dfs);*/
}

void Model::importAnimation(const char* filename) {
	animator.importAnimation(filename);
}

void Model::exportAnimtion(const char* filename, const char* animation) {
	animator.exportAnimation(filename, animation);
}

void Model::addAnimation(AnimationClip clip) {
	animator.addAnimation(clip);
}

void Model::deleteAnimation(const char* animation) {
	animator.deleteAnimation(animation);
}

void Model::playAnimation(const char* name) {
	animator.play(name);
}

void Model::resetMatrix() {
	modelMatrix = glm::mat4(1.f);
}

void Model::translate(float dx, float dy, float dz) {
	glm::mat4 trans = glm::translate(glm::mat4(1.f), glm::vec3(dx, dy, dz));
	modelMatrix = transform * trans * invTransform;
}

void Model::scale(float dx, float dy, float dz) {
	glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(dx, dy, dz));
	modelMatrix = transform * scale * invTransform;
}

void Model::transformate(float tra[3], float rot[3], float sca[3]) {
	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(tra[0], tra[1], tra[2]));
	glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(sca[0], sca[1], sca[2]));
	glm::mat4 rotate = glm::mat4(1.f);
	for (int i = 0; i < 3; i++) {
		rotate *= glm::rotate(glm::mat4(1.f), glm::radians(rot[i]), glm::vec3(i == 0, i == 1, i == 2));
	}

	modelMatrix = transform * translate * rotate * scale * invTransform;
}

void Model::update(float dt) {
	animator.update(dt);
}

Model::Model() {
	prevModelMatrix = modelMatrix = glm::mat4(1.f);
}

Model::Model(std::string& path) {
	prevModelMatrix = modelMatrix = glm::mat4(1.f);
	this->loadModel(path);
}

void Model::setModel(const char* filename) {
	this->loadModel(filename);
}

void Model::drawPrevVelocity(const Shader& shader) {
	for (auto& mesh : meshes) {
		mesh.drawPrevVelocity(shader, animator, modelMatrix, prevModelMatrix, offsets, amount);
	}
}

void Model::drawDepthMap(const Shader& shader) {
	for (auto& mesh : meshes) {
		mesh.drawDepthMap(shader, animator, modelMatrix, offsets, amount);
	}
}

void Model::draw(const Shader& shader) {
	for (auto& mesh : meshes) {
		mesh.draw(shader, animator, modelMatrix, offsets, amount);
	}
	this->prevModelMatrix = modelMatrix;
}

void Model::drawParticlePrevVeloctiy(const Shader& shader) {
	animator.drawParticlePreVelocity(shader);
}

void Model::drawParticle(const Shader& shader) {
	animator.drawParticle(shader);
}

void Model::setModelAmount(int amount) {
	this->amount = amount;
}

const std::vector<Mesh>& Model::getMeshes() const {
	return this->meshes;
}

const Animator& Model::getAnimator() const {
	return this->animator;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;
	
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}