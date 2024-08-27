#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
//#include <GL/glew.h>
#include <SDL_opengl.h>

class Model {
public:
	std::vector<GLfloat> mVertexData;
	std::vector<GLuint> mIndexBufferData;

	Model(const std::string filename);

private:
    float x, y, z;
    float r = 0.5f;
    float g = 0.5f;
    float b = 0.5f; //default color grijs CHANGE TO RANDOM COLOR PER VERTEX

    int i1, i2, i3; //indexBuffer: vertices making up a triangle
    int t1, t2, t3; //indexBuffer: texture
    int n1, n2, n3; //indexBuffer: normals of a triangle

    char token;
};