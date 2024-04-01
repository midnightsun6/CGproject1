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
	void use();
	const GLuint& getProgram();
};

