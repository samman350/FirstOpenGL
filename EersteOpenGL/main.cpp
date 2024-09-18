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
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <fstream>
#include <string>

// onze eigen biliotheken
#include "stb_image.h"
#include "Camera.hpp"
#include "Model.hpp"
#include "Mesh3D.hpp" // also contains App struct
// #include "shaderprogram.hpp"

// reminder Blender exprts obj w CW winding!

//  Globalz
App gApp;
Mesh3D gMesh1("C:/Users/Samuel/Dropbox/BlenderModels/MyLoopMan/3DModel_LowPoly");
Mesh3D gMesh2("C:/Users/Samuel/Dropbox/BlenderModels/MyCloak/3DModel_LowPoly");

void GetOpenGLVersionInfo() {
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

///ERROR STUFF
// error stuff (static void betekent functie alleen calable uit deze file)
static void GLClearAllErrors() {
    while (glGetError() != GL_NO_ERROR) {

    }
}

static bool GLCheckErrorStatus(const char* function, int line) { // true if error
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL fout:" << error << "\tLine:" << __LINE__ << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x,__LINE__); // wrap GLCheck() om een functie heen om te debuggen

// END ERROR STUFF

// INITIALIZE STUFF

void InitializeProgram(App* app) {

    std::cout << "Starting up this FANTASTIC 3D program, WELCOME EVERYBODY" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Probleem met SDL2 vid subsystem toch" << std::endl;
        exit(1);
    }
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    app->mGraphicsApplicationWindow = SDL_CreateWindow("Banaantjeman Windowtje", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app->mScreenWidth, app->mScreenHeight, SDL_WINDOW_OPENGL);

    if (app->mGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL_Window not created" << std::endl;
        exit(1);
    }

    app->mOpenGLContext = SDL_GL_CreateContext(app->mGraphicsApplicationWindow);
    if (app->mOpenGLContext == nullptr) {
        std::cout << "no openGL context\n";
        exit(1);
    }

    glewExperimental = GL_TRUE; // Enable GLEW experimental to ensure modern OpenGL functions are available
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(glewError) << std::endl;
        exit(1);
    }

    GetOpenGLVersionInfo();
}

// hier associeren we de mesh met een pijpleiding
void MeshSetPipeline(Mesh3D* mesh, GLuint pipeline) {
    mesh->mPipeline = pipeline;
}

void PreDraw(){
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gApp.mScreenWidth, gApp.mScreenHeight);
    glClearColor(0.99f, 0.89f, 0.91f, 1.f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

int FindUniformLocation(GLuint pipeline, const GLchar* name){      // deze functie is gewoon glGetUniformLocation + error handling
    // Gebruik naam van uniform in Shader program, niet in CPU program!!
    GLint location = glGetUniformLocation(pipeline, name); 
    if (location < 0) {
        std::cout << "could not find the uniform locashion of " << name << std::endl;
        exit(EXIT_FAILURE);
    }
    return location;
}

// functies voor model transformaties
void MeshTranslate(Mesh3D* mesh, float x, float y, float z){
    mesh->mTransform.model = glm::translate(mesh->mTransform.model, glm::vec3(x, y, z)); // TRANSLATE
}

void MeshRotate(Mesh3D* mesh, float angle, glm::vec3 axis) {
    mesh->mTransform.model = glm::rotate(mesh->mTransform.model, glm::radians(angle), axis); // ROTATE
}

void MeshScale(Mesh3D* mesh, float xscale, float yscale, float zscale){
    mesh->mTransform.model = glm::scale(mesh->mTransform.model, glm::vec3(xscale,yscale,zscale)); // SCALE
}

// SHADER LOADING STUFF 

std::string LoadShaderAsString(const std::string& filename) {
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            result += line + '\n';
        }
        myFile.close();
    }

    return result;
}

GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject;

    if (type == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (type == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);

    glCompileShader(shaderObject);

    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource) {
    GLuint programObject = glCreateProgram();

    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    //valideer:
    glValidateProgram(programObject); // dit is het volle gecompilede shader programma
    // glDetachShader, glDeleteShader

    return programObject;
}

void CreateGraphicsPipeline() {

    std::string vertexShaderSource = LoadShaderAsString("C:/Users/Samuel/source/repos/EersteOpenGL/EersteOpenGL/shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("C:/Users/Samuel/source/repos/EersteOpenGL/EersteOpenGL/shaders/frag.glsl");

    gApp.mGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

// START LOOP BY REGISTERING KEYS

void Input() {
    static int mouseX = gApp.mScreenWidth/2;
    static int mouseY = gApp.mScreenHeight/2;

    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "DoeiDoei" << std::endl;
            gApp.mQuit = true;
        }else if (e.type == SDL_MOUSEMOTION){
            mouseX += e.motion.xrel;
            mouseY += e.motion.yrel;
            gApp.mCamera.MouseLook(mouseX,mouseY);
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float speed = 0.01f;
    if (state[SDL_SCANCODE_UP]) {
        gApp.mCamera.moveForward(speed);
    }
    if (state[SDL_SCANCODE_DOWN]) {
        gApp.mCamera.moveBackward(speed);
    }
    if (state[SDL_SCANCODE_LEFT]) {
        gApp.mCamera.moveLeft(speed);
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        gApp.mCamera.moveRight(speed);
    }
    if (state[SDL_SCANCODE_ESCAPE]) {
        gApp.mQuit = true;
    }
}

void MainLoop() {
    SDL_WarpMouseInWindow(gApp.mGraphicsApplicationWindow,gApp.mScreenWidth/2,gApp.mScreenHeight/2);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (!gApp.mQuit) {
        Input();

        PreDraw(); // achtergrond en zo

        static float rotate = 0.1f;

        //MeshRotate(&gMesh1, rotate, glm::vec3{ 0.f,1.f,0.f });
        MeshRotate(&gMesh2, rotate, glm::vec3{ 0.f,1.f,0.f });

        // draw meshes
        gMesh1.Draw(&gApp);
        gMesh2.Draw(&gApp);

        SDL_GL_SwapWindow(gApp.mGraphicsApplicationWindow); // update screen
    }
}

void CleanUp() {
    SDL_DestroyWindow(gApp.mGraphicsApplicationWindow);
    gApp.mGraphicsApplicationWindow = nullptr;

    glDeleteBuffers(1, &gMesh1.mVertexBufferObject);
    glDeleteVertexArrays(1, &gMesh1.mVertexArrayObject);

    glDeleteProgram(gApp.mGraphicsPipelineShaderProgram);

    SDL_Quit();
}

// deze loop gebeurt maar 1 keer! Als je iets dynamisch wil moet je naar MainLoop()
int main(int argc, char* argv[])
{
    InitializeProgram(&gApp);
    //set up camera
    gApp.mCamera.SetProjectionMatrix(glm::radians(45.0f),(float)gApp.mScreenWidth/ (float)gApp.mScreenHeight,0.1f,1000.0f);

    gMesh1.Create();
    gMesh2.Create();

    //MeshTranslate(&gMesh1, 0.f, 0.f, -1.f);
    //MeshScale(&gMesh1, 1.f, 1.f, 1.f);

    //MeshCreate(&gMesh2);

    MeshTranslate(&gMesh2, 0.f, 0.f, -10.f);
    //MeshScale(&gMesh2, 1.f, 1.f, 1.f);

    CreateGraphicsPipeline();

    // assign pipeline to each mesh
    MeshSetPipeline(&gMesh1, gApp.mGraphicsPipelineShaderProgram);
    MeshSetPipeline(&gMesh2, gApp.mGraphicsPipelineShaderProgram);

    MainLoop();

    CleanUp();

    return 0;
}