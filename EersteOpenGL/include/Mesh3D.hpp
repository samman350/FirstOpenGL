#pragma once

// GLM : wiskunde bibliotheek
#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

// opengl en standaard stuff
#include <iostream>
#include <vector>
#include <SDL.h>
#include <glew.h>
#include <SDL_opengl.h>
#include <string>

// onze eigen biliotheken
#include "stb_image.h"
#include "Camera.hpp"

class Model;

struct App {
    const GLint     mScreenWidth = 1024, mScreenHeight = 768;
    SDL_Window*     mGraphicsApplicationWindow = nullptr;
    SDL_GLContext   mOpenGLContext = nullptr;
    bool            mQuit = false; // if true, quit
    GLuint          mGraphicsPipelineShaderProgram = 0;
    int             FindUniformLocation(GLuint pipeline, const GLchar* name);
    Camera          mCamera; // aha dus acces via App.mCamera.moveForward()? yep
};

struct Transform {
    glm::mat4 model = glm::mat4(1.0f);
    //glm::vec3 translation;
};

struct Mesh3D {
    GLuint          mVertexArrayObject = 0;
    GLuint          mVertexBufferObject = 0;
    GLuint          mIndexBufferObject = 0;
    GLuint          mTextureObject;
    GLuint          mPipeline = 0; // graphics pipeline die gebruikt wordt met dit mesh
    Transform       mTransform;
    float           m_uOffset = -4.f;
    float           m_uRotate = 0.f;
    float           m_uScale = 1.f;

    // Constructor: load model on the heap, using smart pointer:
    std::unique_ptr<Model> mModel;
    Mesh3D(const std::string& ObjPath) {
                    mModel = std::make_unique<Model>(ObjPath);
    } //destructor functie niet nodig vanwege smart pointer.

    void            Create();
    void            Draw(App* app);
};