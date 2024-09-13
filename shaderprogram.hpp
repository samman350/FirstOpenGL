#pragma once

#include <iostream>
#include <fstream>
#include <string>

class shaderProgram {
public:
	std::string LoadShaderAsString(const std::string& filename);
	GLuint CompileShader(GLuint type, const std::string& source);
	GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource);
	void CreateGraphicsPipeline();
};