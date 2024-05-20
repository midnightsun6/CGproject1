#pragma once
#include "Definition.h"

struct ShaderInfo {
	GLenum type;
	const char* filename;
	GLuint shader;
};

class Shader {
private:
	GLuint program;
	
	const GLchar* ReadShader(const char*);
	GLuint LoadShaders(ShaderInfo*);

public:
	Shader();
	Shader(const char* vert, const char* frag);
	Shader(const char* vert, const char* geo, const char* frag);
	void setShader(const char* vert, const char* frag);
	void setShader(const char* vert, const char* geo, const char* frag);

	void use() const;
	template<class T>
	inline void setUniform(const std::string& name, const T& value) const;
	template<>
	inline void setUniform(const std::string& name, const bool& value) const;
	template<>
	inline void setUniform(const std::string& name, const int& value) const;
	template<>
	inline void setUniform(const std::string& name, const unsigned int& value) const;
	template<>
	inline void setUniform(const std::string& name, const float& value) const;
	template<>
	inline void setUniform(const std::string& name, const glm::mat4& value) const;
	template<>
	inline void setUniform(const std::string& name, const glm::vec3& value) const;

	const GLuint& getProgram() const;
};

template<class T>
inline void Shader::setUniform(const std::string& name, const T& value) const {}

template<>
inline void Shader::setUniform(const std::string& name, const bool& value) const {
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), (int)value);
}

template<>
inline void Shader::setUniform(const std::string& name, const int& value) const {
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}

template<>
inline void Shader::setUniform(const std::string& name, const unsigned int& value) const {
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}

template<>
inline void Shader::setUniform(const std::string& name, const float& value) const {
	glUniform1f(glGetUniformLocation(this->program, name.c_str()), value);
}

template<>
inline void Shader::setUniform(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
inline void Shader::setUniform(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(this->program, name.c_str()), 1, glm::value_ptr(value));
}