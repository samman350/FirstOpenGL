#include "Mesh3D.hpp"
#include "Model.hpp"

// GLM : wiskunde bibliotheek
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

// opengl en standaard stuff
#include <iostream>
#include <vector>
#include <SDL_opengl.h>
#include <string>

int App::FindUniformLocation(GLuint pipeline, const GLchar* name) {      // deze functie is gewoon glGetUniformLocation + error handling
    // Gebruik naam van uniform in Shader program, niet in CPU program!!
    GLint location = glGetUniformLocation(pipeline, name);
    if (location < 0) {
        std::cout << "could not find the uniform locashion of " << name << std::endl;
        exit(EXIT_FAILURE);
    }
    return location;
}

void Mesh3D::Draw(App* app) {

    glUseProgram(mPipeline);

    // VIEW MATRIX AS UNIFORM

    glm::mat4 view = app->mCamera.GetViewMatrix();
    GLint location_ViewMat = app->FindUniformLocation(app->mGraphicsPipelineShaderProgram, "u_View");
    glUniformMatrix4fv(location_ViewMat, 1, false, &view[0][0]); // die laatste pointer wijst naar het eerste element van de matrix

    // TRANSLATION, ROTATION AND SCALE MATRIX AS UNIFORM

    GLint location_ModelMat = app->FindUniformLocation(app->mGraphicsPipelineShaderProgram, "u_ModelMatrix");
    glUniformMatrix4fv(location_ModelMat, 1, false, &mTransform.model[0][0]); // die laatste pointer wijst naar het eerste element van de matrix

    // PROJECTION MATRIX AS UNIFORM

    glm::mat4 projection = app->mCamera.GetProjectionMatrix();
    GLint location_ProjMat = app->FindUniformLocation(app->mGraphicsPipelineShaderProgram, "u_Projection");
    glUniformMatrix4fv(location_ProjMat, 1, false, &projection[0][0]); // die laatste pointer wijst naar het eerste element van de matrix

    // TIME AS UNIFORM

    //float timeVal = (float)SDL_GetTicks(); // time in MILLIseconds
    //GLint location_Time = FindUniformLocation(app->mGraphicsPipelineShaderProgram, "u_Time");
    //glUniform1f(location_Time, timeVal);

    // TEXTURE AS UNIFORM
    glActiveTexture(GL_TEXTURE0);  // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, mTextureObject);  // Bind the mesh-specific texture
    glUniform1i(app->FindUniformLocation(app->mGraphicsPipelineShaderProgram, "uTexture"), 0);

    // Draw stuff

    glBindVertexArray(mVertexArrayObject); // select/enable VAO
    // glDrawElements(GL_TRIANGLES, vertexcount, GL_UNSIGNED_INT, 0); // draw using vertexbuffer
    //std::cout << "how much i will draw: " << mModel->mVertexData.size() / 8 << std::endl;
    glDrawArrays(GL_TRIANGLES, 0, mModel->mVertexData.size() / 8);

    glUseProgram(0);

}

void Mesh3D::Create() {
    std::cout << "VertexData:" << std::endl;
    //for (int i = 0; i < gTestModel->mVertexData.size(); i++) {
    for (int i = 0; i < 8; i++) {
        std::cout << mModel->mVertexData[i] << std::endl;
    }

    // De bind vertex array wrapped eigenlijk om alle andere data heen - vertex, index, en texture data
    glGenVertexArrays(1, &mVertexArrayObject); // mesh is een  pointer dus members worden als -> gegeven
    glBindVertexArray(mVertexArrayObject);

    // start met genereren van Vertex Buffer Object

    // VERTEX BUFFER 
    glGenBuffers(1, &mVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
    //glBufferData(GL_ARRAY_BUFFER, gTestModel->mVertexData.size() * sizeof(GLfloat), gTestModel->mVertexData.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, mModel->mVertexData.size() * sizeof(GLfloat), mModel->mVertexData.data(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    //const std::vector<GLuint> indexBufferData{ 0, 1, 2, 3, 2, 1 }; // CCW orientatie

    // INDEX BUFFER
    //glGenBuffers(1, &mIndexBufferObject);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferObject);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, gTestModel->mIndexBufferData.size() * sizeof(GLuint), gTestModel->mIndexBufferData.data(), GL_STATIC_DRAW);

    // TEXTURE BUFFER (maybe make different function, to loop over different textures)
    std::cout << "texWidth: " << mModel->texWidth << ", texHeight: " << mModel->texHeight << ", texNrChannels: " << mModel->texNrChannels << std::endl;
    glGenTextures(1, &mTextureObject);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureObject);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (mModel->data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mModel->texWidth, mModel->texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, mModel->data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture bound" << std::endl;
    }
    else {
        std::cout << "no texture bound" << std::endl;
    }
    stbi_image_free(mModel->data);

    // Tell what is what in buffers, start with VERTICES
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

    //glGenBuffers(1, &gVertexBufferObject2);
    //glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject2); // bind is iets als 'select', refereert naar de state machine
    //glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(GLfloat), vertexColors.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1); // COLORS
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // hiermee vertel je de gpu hoe de buffer geinterpreteerd moet worden
    //kleur zit 3 plaatsen opgeschoven vergeleken met xyz, daarom offset in laatste argument

    glEnableVertexAttribArray(2); // TEXTURE COORDINATES
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}


