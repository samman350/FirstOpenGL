#include "Model.hpp"

// constructor voor Model class: laad het model, en vul mVertexData en mIndexBufferData met waarden.
// later komen hier ook nog normals en texture coordinates bij.

Model::Model(const std::string filename) {
    std::string line = "";
    std::ifstream myFile(filename);  // TO DO: ERROR CHECK!

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            if (line.substr(0, 2) == "v ") {
                sscanf_s(line.c_str(), "%c %f %f %f", &token, 1, &x, &y, &z); // '1 ' is byte size of %c
                mVertexData.push_back((GLfloat)x);
                mVertexData.push_back((GLfloat)y);// zelde als matlab a = [a y];
                mVertexData.push_back((GLfloat)z);
                mVertexData.push_back((GLfloat)r);
                mVertexData.push_back((GLfloat)g);
                mVertexData.push_back((GLfloat)b);
            }
            else if (line.substr(0, 2) == "f ") {
                sscanf_s(line.c_str(), "%c %d/%d/%d %d/%d/%d %d/%d/%d", &token, 1, &i1, &t1, &n1,
                    &i2, &t2, &n2,
                    &i3, &t3, &n3);
                mIndexBufferData.push_back((GLuint)i1 - 1); // -1 vanwege zero indexing in c++ vergeleken met .obj 1 index
                mIndexBufferData.push_back((GLuint)i2 - 1);
                mIndexBufferData.push_back((GLuint)i3 - 1);
            }
        }
    myFile.close();
    }
}