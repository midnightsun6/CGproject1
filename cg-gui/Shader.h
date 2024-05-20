#pragma once
#include "Definition.h"

class Shader {
private:
	GLuint program;
	
	const GLchar* ReadShader(const char*);
	GLuint LoadShaders(ShaderInfo*);

public:
	Shader();
	Shader(const char* vert, const char* frag);
	void setShader(const char* vert, const char* frag);
	void setShader(const char* vert, const char* geo, const char* frag);

	void use();
	void setBool(const std::string& name, const bool& value) const;
	void setInt(const std::string& name, const int& value) const;
	void setUint(const std::string& name, const unsigned int& value) const;
	void setFloat(const std::string& name, const float& value) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;

	const GLuint& getProgram() const;
};

