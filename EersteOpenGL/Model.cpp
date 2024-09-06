#include "Model.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// constructor voor Model class: laad het model, en vul mVertexData en mIndexBufferData met waarden.
// later komen hier ook nog normals en texture coordinates bij.

Model::Model(const std::string filename) {
    std::string line = "";
    std::ifstream myFile(filename);  // TO DO: ERROR CHECK!
    // Readout File, store stuff in private vectors

    std::cout << "nu ga ik de file lezen!" << std::endl;

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            if (line.substr(0, 2) == "v ") {
                sscanf_s(line.c_str(), "%c %f %f %f", &token, 1, &x, &y, &z); // '1 ' is byte size of %c
                mPreVertexData.push_back((GLfloat)x);
                mPreVertexData.push_back((GLfloat)y);// zelde als matlab a = [a y];
                mPreVertexData.push_back((GLfloat)z);
                
                mPreVertexData.push_back((GLfloat)(rand()) / (GLfloat)(RAND_MAX)); // random colors :)
                mPreVertexData.push_back((GLfloat)(rand()) / (GLfloat)(RAND_MAX));
                mPreVertexData.push_back((GLfloat)(rand()) / (GLfloat)(RAND_MAX));
                //mVertexData.push_back((GLfloat)r);
                //mVertexData.push_back((GLfloat)g);
                //mVertexData.push_back((GLfloat)b);
            }
            else if(line.substr(0,3) == "vt ") {
                sscanf_s(line.c_str(), "%2s %f %f", &token2, (int)(sizeof(token2) / sizeof(token2[0])), &u, &v); // '1 ' is byte size of %2c
                mTextureData.push_back((GLfloat)u);
                mTextureData.push_back((GLfloat)v);
            }
            else if (line.substr(0, 2) == "f ") {
                sscanf_s(line.c_str(), "%c %d/%d/%d %d/%d/%d %d/%d/%d", &token, 1, &i1, &t1, &n1,
                    &i2, &t2, &n2,
                    &i3, &t3, &n3);
                mVertIndex.push_back((GLuint)i1 - 1); // -1 vanwege zero indexing in c++ vergeleken met .obj 1 index
                mVertIndex.push_back((GLuint)i2 - 1);
                mVertIndex.push_back((GLuint)i3 - 1);
                // face texture indices:
                mTexIndex.push_back((GLuint)t1 - 1);
                mTexIndex.push_back((GLuint)t2 - 1);
                mTexIndex.push_back((GLuint)t3 - 1);
            }
        }
    myFile.close();
    }

    // construeer de index buffer object en het vertex buffer object.
    // do this on the base of unique combinations f v/vt. If unique: new index and new entry in VBO (xyz rgb uv), 
    // if not: push old index and do not update VBO. NO

    // construeer alleen array (x y z | r g b | u v).

    std::cout << "nu ga ik de vertexArray en indexArray opzetten!" << std::endl;

    for (int i = 0; i < mVertIndex.size(); i++){
        mVertexData.push_back(mPreVertexData[(mVertIndex[i]) * 6]);    // vertices
        mVertexData.push_back(mPreVertexData[(mVertIndex[i]) * 6 + 1]);
        mVertexData.push_back(mPreVertexData[(mVertIndex[i]) * 6 + 2]);
        mVertexData.push_back(mPreVertexData[(mVertIndex[i]) * 6 + 3]);// color
        mVertexData.push_back(mPreVertexData[(mVertIndex[i]) * 6 + 4]);
        mVertexData.push_back(mPreVertexData[(mVertIndex[i]) * 6 + 5]);
        mVertexData.push_back(mTextureData[(mTexIndex[i]) * 2    ]);      // uv coordinates
        mVertexData.push_back(mTextureData[(mTexIndex[i]) * 2 + 1]);
        //std::cout << "counter: " << counter << std::endl;
    }

    std::cout << "supposed amount of vertices: " << (mVertexData.size() / 8) * 3 << std::endl;

    if (mVertexData.size() != 0) {
        std::cout << filename << " loaded!" << std::endl;
    }
    else if (mVertexData.size() == 0) {
        std::cout << "obj model not loaded!" << std::endl;
    }

    // load texture image:
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("C:/Users/Samuel/Documents/bebson.jpg", &texWidth, &texHeight, &texNrChannels, 0); //FIX PATH!!
    //std::cout << "texWidth: " << texWidth << ", texHeight: " << texHeight << ", texNrChannels: " << texNrChannels << std::endl;
}