#include "LoadShader.h"

const GLchar* ReadShader(const char* filename) {
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

GLuint LoadShaders(ShaderInfo* shaders) {
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
