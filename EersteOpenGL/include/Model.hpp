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

    int texWidth, texHeight, texNrChannels;
    unsigned char* data; // texture image data

	Model(const std::string filename);

private:
    std::vector<GLfloat> mPreVertexData;
    std::vector<GLfloat> mTextureData;
    std::vector<GLuint> mVertIndex;
    std::vector<GLuint> mTexIndex;

    float x, y, z;
    //GLfloat r = (GLfloat)(rand()) / (GLfloat)(RAND_MAX);
    //GLfloat g = (GLfloat)(rand()) / (GLfloat)(RAND_MAX);
    //GLfloat b = (GLfloat)(rand()) / (GLfloat)(RAND_MAX); //default color grijs CHANGE TO RANDOM COLOR PER VERTEX

    float u, v; //texture uv coordinates
    
    int i1, i2, i3; //indexBuffer: vertices making up a triangle
    int t1, t2, t3; //indexBuffer: texture indices
    int n1, n2, n3; //indexBuffer: normals indices

    bool unique = true;
    int  counter = 0;
    char token;
    char token2[3];
};