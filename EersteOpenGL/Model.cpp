#include "Model.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>

// constructor voor Model class: laad het model, en vul mVertexData en mIndexBufferData met waarden.
// later komen hier ook nog normals en texture coordinates bij.

Model::Model(const std::string filename) {
    std::string line = "";
    // std::basic_string<char> mObjExt = ".obj";
    const std::string mObjExt = ".obj";
    const std::string filenameObj = filename + mObjExt;
    std::ifstream myFile(filenameObj);  // TO DO: ERROR CHECK!
    // Readout File, store stuff in private vectors

    std::cout << "nu ga ik de file lezen!" << std::endl;

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            if (line.substr(0, 2) == "v ") {
                sscanf_s(line.c_str(), "%c %f %f %f", &token, 1, &x, &y, &z); // '1 ' is byte size of %c
                mPreVertexData.insert(mPreVertexData.end(), { (GLfloat)x, (GLfloat)y, (GLfloat)z, // first xyz, then colors:
                    (GLfloat)(rand()) / (GLfloat)(RAND_MAX) ,(GLfloat)(rand()) / (GLfloat)(RAND_MAX) ,(GLfloat)(rand()) / (GLfloat)(RAND_MAX) });
            }
            else if(line.substr(0,3) == "vt ") {
                sscanf_s(line.c_str(), "%2s %f %f", &token2, (int)(sizeof(token2) / sizeof(token2[0])), &u, &v); // (int).. is byte size of %2s
                mTextureData.insert(mTextureData.end(), { (GLfloat)u, (GLfloat)v });
            }
            else if (line.substr(0, 2) == "f ") {
                sscanf_s(line.c_str(), "%c %d/%d/%d %d/%d/%d %d/%d/%d", &token, 1, &i1, &t1, &n1,
                    &i2, &t2, &n2,
                    &i3, &t3, &n3);
                // face vertex indices
                mVertIndex.insert(mVertIndex.end(), { (GLuint)i1 - 1 ,(GLuint)i2 - 1 ,(GLuint)i3 - 1 }); // -1 vanwege zero indexing in c++ vergeleken met .obj 1 index
                // face texture indices:
                mTexIndex.insert(mTexIndex.end(), { (GLuint)t1 - 1, (GLuint)t2 - 1, (GLuint)t3 - 1 });
            }
        }
    myFile.close();
    }

    // construeer de index buffer object en het vertex buffer object.
    // do this on the base of unique combinations f v/vt. If unique: new index and new entry in VBO (xyz rgb uv), 
    // if not: push old index and do not update VBO. NO

    // construeer alleen array (x y z | r g b | u v).

    std::cout << "nu ga ik de vertexArray!" << std::endl;

    for (int i = 0; i < mVertIndex.size(); i++){
        mVertexData.insert(mVertexData.end(), { mPreVertexData[(mVertIndex[i]) * 6], mPreVertexData[(mVertIndex[i]) * 6 + 1],// vertices
                                            mPreVertexData[(mVertIndex[i]) * 6 + 2], mPreVertexData[(mVertIndex[i]) * 6 + 3],// color
                                            mPreVertexData[(mVertIndex[i]) * 6 + 4], mPreVertexData[(mVertIndex[i]) * 6 + 5],
                                               mTextureData[(mTexIndex[i]) * 2] ,  mTextureData[(mTexIndex[i]) * 2 + 1] }); // uv coordinates
    }

    std::cout << "supposed amount of vertices: " << (mVertexData.size() / 8) * 3 << std::endl;

    if (mVertexData.size() != 0) {
        std::cout << filename << " loaded!" << std::endl;
    }
    else if (mVertexData.size() == 0) {
        std::cout << "obj model not loaded!" << std::endl;
    }

    // load texture image:
    const std::string mTexExt = ".jpg";
    const std::string filenameTex = filename + mTexExt;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(filenameTex.c_str(), &texWidth, &texHeight, &texNrChannels, 0);
    //std::cout << "texWidth: " << texWidth << ", texHeight: " << texHeight << ", texNrChannels: " << texNrChannels << std::endl;
}