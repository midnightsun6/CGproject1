#pragma once
#include "Definition.h"

class Shader {
private:
	GLuint program;
	std::unordered_map<std::string, glm::mat4> mat4Map;
	
	const GLchar* ReadShader(const char*);
	GLuint LoadShaders(ShaderInfo*);

public:
	Shader();
	Shader(const char* vert, const char* frag);
	void setShader(const char* vert, const char* frag);

	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setUint(const std::string& name, unsigned int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;

	glm::mat4 getMat4(std::string name) const;
	const GLuint& getProgram() const;
};

