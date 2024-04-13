#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

void Model::loadModel(std::string filename) {
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
	this->resortMesh();
	std::cout << "Loading model Successfully: " << filename << '\n';
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// Add all node in meshes.
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		//std::cout << aimesh->mName.C_Str() << '\n';
		
		// Get mesh
		Mesh mesh = this->processMesh(aimesh, scene);
		meshTable[mesh.getName()] = mesh;
		parts.push_back(mesh.getName());
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
		Mesh& mesh = meshes[i];
		std::string parentName = mesh.getName();
		int lastSpaceIdx = parentName.find_last_of(' ');
		if (lastSpaceIdx == std::string::npos) {
			continue;
		}
		parentName = parentName.substr(0, lastSpaceIdx);

		if (meshTable.count(parentName)) {
			meshTable[parentName].addChild(meshTable[mesh.getName()]);
			meshTable.erase(mesh.getName());
		}
	}

	meshes.clear();
	for (auto& [_, mesh] : meshTable) {
		meshes.push_back(mesh);
	}
}

void Model::addAnimation(const char* filename) {
	animator.addAnimation(filename);
}

void Model::playAnimation(const char* name) {
	animator.play(name);
}

void Model::update(float dt) {
	animator.update(dt);
}

Model::Model() {}

Model::Model(std::string& path) {
	this->loadModel(path);
}

void Model::setModel(const char* filename) {
	this->loadModel(filename);
}

void Model::draw(Shader& shader) {
	for (auto& mesh : meshes) {
		mesh.draw(shader, animator, glm::mat4(1.f));
	}
}

const std::vector<std::string>& Model::getPartsName() {
	return this->parts;
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