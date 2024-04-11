#include "Shader.h"

const GLchar* Shader::ReadShader(const char* filename) {
	FILE* infile = fopen(filename, "rb");
	if (!infile) {
		std::cout << "Unable to open file '" << filename << "'\n";
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = new GLchar[len + 1];
	fread(source, 1, len, infile);
	fclose(infile);

	source[len] = 0;
	return const_cast<const GLchar*>(source);
}

GLuint Shader::LoadShaders(ShaderInfo* shaders) {
	if (shaders == NULL) {
		return -1;
	}

	GLint program = glCreateProgram();
	ShaderInfo* entry = shaders;
	while (entry->type != GL_NONE) {
		GLuint shader = glCreateShader(entry->type);
		entry->shader = shader;
		const GLchar* source = ReadShader(entry->filename);
		if (source == NULL) {
			std::cout << "source is null\n";
		}

		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLchar infoLog[1024];
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "Error compiling shader type " << entry->type << ": " << infoLog;
		}

		glAttachShader(program, shader);
		entry++;
	}

	glLinkProgram(program);
	return program;
}

Shader::Shader() {}

Shader::Shader(const char* vert, const char* frag) {
	this->setShader(vert, frag);
}

void Shader::setShader(const char* vert, const char* frag) {
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, vert },
		{ GL_FRAGMENT_SHADER, frag },
		{ GL_NONE, NULL }
	};
	program = LoadShaders(shaders);
}

void Shader::use() {
	glUseProgram(program);
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}
void Shader::setUint(const std::string& name, unsigned int value) const {
	glUniform1i(glGetUniformLocation(this->program, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(this->program, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const {
	glUniformMatrix4fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(this->program, name.c_str()), 1, glm::value_ptr(value));
}

const GLuint& Shader::getProgram() const {
	return program;
}
